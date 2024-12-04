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
#include <SDL2/SDL.h>


#define PORT 50000
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

/*
typedef struct thData{
    int idThread; //id-ul thread-ului tinut in evidenta de acest program
    int cl; //descriptorul intors de accept
}thData;
*/

typedef struct {
    int client1; //socket pentru primul client
    int client2; //socket pt al 2 lea client
    int idPair; //id-ul perechii;
} ClientPair;

ClientPair pairs[MAX_CLIENTS / 2];
int pair_count = 0;
pthread_mutex_t pair_lock = PTHREAD_MUTEX_INITIALIZER;

static void *handle_pair(void *a);

bool raspunde(void *);

int main() {
    struct sockaddr_in server;    // structura folosita de server
    struct sockaddr_in from;    
    int sd;        //descriptorul de socket 
    pthread_t threads[MAX_CLIENTS / 2];    //identificatorii thread-urilor care se vor crea
    int i = 0;

    /* crearea unui socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[server]Eroare la socket().\n");
        exit(EXIT_FAILURE);
    }

    /* utilizarea optiunii SO_REUSEADDR */
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    /* pregatirea structurilor de date */
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    /* umplem structura folosita de server */
    server.sin_family = AF_INET;    
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    /* atasam socketul */
    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
        perror("[server]Eroare la bind().\n");
        exit(EXIT_FAILURE);
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen(sd, 10) == -1) {
        perror("[server]Eroare la listen().\n");
        exit(EXIT_FAILURE);
    }

    printf("[server]Asteptam la portul %d...\n", PORT);
    fflush(stdout);

    while (1) {
        int client;
        //thData *td; //parametru functia executata de thread     
        int length = sizeof(from);

        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0) {
            perror("[server]Eroare la accept().\n");
            continue;
        }

        printf("[server]Client conectat de la %s:%d\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port));

        pthread_mutex_lock(&pair_lock);
        if(pair_count % 2 == 0) {
            //se creeaza o pereche noua
            pairs[pair_count / 2].client1 = client;
            pairs[pair_count / 2].idPair = i++;
        }else {
            //se completeaza structul perechii actuale
            pairs[pair_count / 2].client2 = client;

            //se creaza un nou thread pt aceasta pereche, fiind completa
            pthread_create(&threads[pair_count / 2], NULL, handle_pair, &pairs[pair_count / 2]);
        }
        pair_count+=1;
        pthread_mutex_unlock(&pair_lock);
    }
    close(sd);    
    return 0;
}

static void *handle_pair(void *a) {
    ClientPair *pair = (ClientPair *)a;
    int client1 = pair->client1;
    int client2 = pair->client2;
    char buffer[BUFFER_SIZE];

    printf("[pair %d] Pereche creată: client1=%d, client2=%d\n", pair->idPair, client1, client2);

    while (1) {
        // Citim mesaj de la client1
        ssize_t bytesRead = read(client1, buffer, BUFFER_SIZE);
        if (bytesRead <= 0) {
            printf("[pair %d] Client1 deconectat.\n", pair->idPair);
            break;
        }
        buffer[bytesRead] = '\0';

        if(strcmp(buffer, "exit") == 0) {
            printf("[perechea %d] Clientul 1 s-a deconectat, deci a castigat client 2\n", pair->idPair);
            if(write(client2, "exit", 5) == -1) {
                perror("Eroare la trimitere exit in client2\n");
            }
            break;
        }

        printf("[pair %d] Client1: %s\n", pair->idPair, buffer);

        if(write(client1, buffer, sizeof(buffer)) == -1) {
            perror("Eroare la scriere in buffer\n");
        }

        // Citim mesaj de la client2
        bzero(buffer, sizeof(buffer));
        bytesRead = read(client2, buffer, BUFFER_SIZE);
        if (bytesRead <= 0) {
            printf("[pair %d] Client2 deconectat.\n", pair->idPair);
            break;
        }
        buffer[bytesRead] = '\0';

        if(strcmp(buffer, "exit") == 0) {
            printf("[perechea %d] Clientul 2 s-a deconectat, deci a castigat client 1\n", pair->idPair);
            if(write(client1, "exit", 5) == -1) {
                perror("Eroare la trimitere exit in client1\n");
            }
            break;
        }

        printf("[pair %d] Client2: %s\n", pair->idPair, buffer);

        if(write(client2, buffer, sizeof(buffer)) == -1) {
            perror("Eroare la scriere in buffer\n");
        }
    }

    close(client1);
    close(client2);
    return NULL;
}


