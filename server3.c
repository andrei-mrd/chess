#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "chess.h"
#include <semaphore.h>


#define PORT 50000
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

typedef struct {
    int client1; // socket pentru primul client
    int client2; // socket pentru al doilea client
    int idPair;  // ID-ul perechii
} ClientPair;

typedef struct {
    char move[BUFFER_SIZE];
    int turn; // Cine este la rând: 0 = Thread 1 (client1), 1 = Thread 2 (client2)
    int socket1; // Socket pentru client1
    int socket2; // Socket pentru client2
    pthread_mutex_t mutex;
    pthread_cond_t cond_sender;
    pthread_cond_t cond_receiver;
} shared_data;


ClientPair pairs[MAX_CLIENTS / 2]; // Perechile de clienți
int pair_count = 0;                 // Contor pentru perechi
pthread_mutex_t pair_lock = PTHREAD_MUTEX_INITIALIZER; // Mutex pentru sincronizare

void *thread_1(void *arg) {
    shared_data *data = (shared_data *)arg;
    bool running = true;

    while (running) {
        pthread_mutex_lock(&data->mutex);

        // Așteaptă până când este rândul său
        while (data->turn != 0) {
            pthread_cond_wait(&data->cond_sender, &data->mutex);
        }

        // Primește mutarea de la clientul 1
        memset(data->move, 0, BUFFER_SIZE);
        int result = recv(data->socket1, data->move, BUFFER_SIZE, 0);
        if (result <= 0) {
            printf("Clientul s-a deconectat.\n");
            close(data->socket1);
            close(data->socket2);
            running = false;
            pthread_cond_signal(&data->cond_receiver);
            pthread_mutex_unlock(&data->mutex);
            break;
        }

        data->move[result] = '\0';
        printf("[Thread 1] Received from client1: %s\n", data->move);

        // Semnalizează că poate să trimită mutarea către clientul 2
        data->turn = 1;
        pthread_cond_signal(&data->cond_receiver); // Semnalizează thread-ul 2 să trimită către clientul 2

        // Așteaptă ca clientul 2 să trimită înapoi un răspuns
        pthread_cond_wait(&data->cond_sender, &data->mutex);

        // La final, procesăm răspunsul primit de la clientul 2
        printf("[Thread 1] Received response from client2: %s\n", data->move);
        if(send(data->socket1, data->move, BUFFER_SIZE, 0) == -1) {
            perror("Eroare la trimiterea raspunsului de la client2 la client1\n");
            close(data->socket1);
            close(data->socket2);
            running = false;
            pthread_cond_signal(&data->cond_receiver);
            pthread_mutex_unlock(&data->mutex);
            break;
        }

        pthread_mutex_unlock(&data->mutex);
    }

    return NULL;
}


void *thread_2(void *arg) {
    shared_data *data = (shared_data *)arg;
    bool running = true;

    while (running) {
        pthread_mutex_lock(&data->mutex);

        // Așteaptă până când este rândul său
        while (data->turn != 1) {
            pthread_cond_wait(&data->cond_receiver, &data->mutex);
        }

        // Trimite mutarea către clientul 2
        if (send(data->socket2, data->move, strlen(data->move), 0) <= 0) {
            perror("Eroare la trimiterea către client2");
            running = false;
        } else {
            printf("[Thread 2] Sent to client2: %s\n", data->move);
        }

        memset(data->move, 0, BUFFER_SIZE);

        // Primește răspunsul de la clientul 2
        int result = recv(data->socket2, data->move, BUFFER_SIZE, 0);
        if (result <= 0) {
            printf("Clientul s-a deconectat.\n");
            close(data->socket1);
            close(data->socket2);
            running = false;
            pthread_cond_signal(&data->cond_receiver); // Semnalizează finalizare
            pthread_mutex_unlock(&data->mutex);
            break;
        }

        data->move[result] = '\0';
        printf("[Thread 2] Received from client2: %s\n", data->move);

        // Semnalizează că poate să primească un alt mesaj
        data->turn = 0;
        pthread_cond_signal(&data->cond_sender); // Semnalizează thread-ului 1 că a primit răspunsul

        pthread_mutex_unlock(&data->mutex);
    }

    return NULL;
}





// Funcția care va gestiona jocul dintre două perechi
static void *handle_pair(void *a);

// Functia pentru initializarea serverului
int init_server();

int main() {
    int sd = init_server();
    struct sockaddr_in from;
    int length = sizeof(from);

    pthread_t threads[MAX_CLIENTS / 2]; // Threaduri pentru fiecare pereche

    printf("[server] Așteptăm clienți la portul %d...\n", PORT);
    while (1) {
        int client;
        if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0) {
            perror("[server] Eroare la accept().\n");
            continue;
        }

        printf("[server] Client conectat de la %s:%d\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port));

        // Alocăm clientul într-o pereche
        pthread_mutex_lock(&pair_lock);
        if (pair_count % 2 == 0) {
            pairs[pair_count / 2].client1 = client;
        } else {
            pairs[pair_count / 2].client2 = client;
            pairs[pair_count / 2].idPair = pair_count / 2;
            pthread_create(&threads[pair_count / 2], NULL, handle_pair, &pairs[pair_count / 2]);
        }
        pair_count++;
        pthread_mutex_unlock(&pair_lock);
    }

    close(sd);
    return 0;
}

int init_server() {
    int sd;
    struct sockaddr_in server;
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[server] Eroare la socket().\n");
        exit(EXIT_FAILURE);
    }

    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
        perror("[server] Eroare la bind().\n");
        exit(EXIT_FAILURE);
    }

    if (listen(sd, 10) == -1) {
        perror("[server] Eroare la listen().\n");
        exit(EXIT_FAILURE);
    }

    return sd;
}

static void *handle_pair(void *a) {
    ClientPair *pair = (ClientPair *)a;
    int client1 = pair->client1;
    int client2 = pair->client2;

     if (write(client1, "A", 1) <= 0) {
        perror("Eroare transmitere culoare client1\n");
        close(client1);
        close(client2);
        return NULL;
    }
    if (write(client2, "N", 1) <= 0) {
        perror("Eroare transmitere culoare client2\n");
        close(client1);
        close(client2);
        return NULL;
    }

    pthread_t t1, t2;
    shared_data data;

    pthread_mutex_init(&data.mutex, NULL);
    pthread_cond_init(&data.cond_sender, NULL);
    pthread_cond_init(&data.cond_receiver, NULL);

    data.turn = 0; // Thread-ul 1 (client1) începe
    data.socket1 = client1;
    data.socket2 = client2;

    // Creare thread-uri
    pthread_create(&t1, NULL, thread_1, &data);
    pthread_create(&t2, NULL, thread_2, &data);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    close(client1);
    close(client2);

    pthread_mutex_destroy(&data.mutex);
    pthread_cond_destroy(&data.cond_sender);
    pthread_cond_destroy(&data.cond_receiver);

    return NULL;
}
