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
#include <semaphore.h>


#define PORT 50000
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

typedef struct {
    int client1; // socket pentru primul client
    int client2; // socket pentru al doilea client
    int idPair;  // ID-ul perechii
} ClientPair;

ClientPair pairs[MAX_CLIENTS / 2]; // Perechile de clienți
int pair_count = 0;                 // Contor pentru perechi
pthread_mutex_t pair_lock = PTHREAD_MUTEX_INITIALIZER; // Mutex pentru sincronizare

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
    char buffer[BUFFER_SIZE];

    printf("[pair %d] Pereche creată: client1=%d, client2=%d\n", pair->idPair, client1, client2);

    int count = 0;

    // Trimitem culorile către clienți
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

    int current_client = client1;
    int opponent_client = client2;
    char current_color = 'A';
    
    bool verificare;

    int from_x = -1, from_y = -1, to_x = -1, to_y = -1;
    int prev_x1, prev_y1, prev_x2, prev_y2;
    ssize_t bytesRead;

    while (1) {
        //primeste info de la client1
        memset(buffer, 0, BUFFER_SIZE);
        bytesRead = recv(client1, buffer, BUFFER_SIZE, 0);
        if(bytesRead <= 0) {
            printf("client1 nu a trimis input\n");
            break;
        }
        buffer[bytesRead] = '\0';
        printf("info : %s\n", buffer);

        //trimite info la client2
        if(send(client2, buffer, BUFFER_SIZE, 0) == -1) {
            perror("eroare la trimitere info de la server la client2\n");
            break;
        }

        //primeste info 1 de la client2
        memset(buffer, 0, BUFFER_SIZE);
        bytesRead = recv(client2, buffer, BUFFER_SIZE, 0);
        if(bytesRead <= 0) {
            printf("client2 nu a trimis input\n");
            break;
        }
        buffer[bytesRead] = '\0';
        printf("info1 : %s\n", buffer);

        //trimite info1 la client1
        if(send(client1, buffer, BUFFER_SIZE, 0) == -1) {
            perror("eroare la trimitere info1 de la server la client1\n");
            break;
        }
    }

    close(client1);
    close(client2);
}