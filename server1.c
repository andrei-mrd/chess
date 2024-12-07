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
    sem_t sem_client1; //semafor pt clientul 1
    sem_t sem_client2;  //semafor pt clientul 2
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
            sem_init(&pairs[pair_count / 2].sem_client1, 0, 1); //semafor activ pentru clientul 1
            sem_init(&pairs[pair_count / 2].sem_client2, 0, 0); //semafor inactiv pt clientul 2 ca inca nu e conectat
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
    int **board = create_chess_board();
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

    while (1) {

        printf("Culoarea curenta e %c si clientul curent e %d\n", current_color, current_client);
        ssize_t bytesRead;

        snprintf(buffer, sizeof(buffer), "%d %d %d %d", from_x, from_y, to_x, to_y);

        if(current_client == client1) {
            sem_wait(&pair->sem_client1);
        } else {
            sem_wait(&pair->sem_client2);
        }
        
        // Citim mutarea de la clientul curent
        while (1) {
            printf("[DEBUG] Așteptăm mutarea de la clientul curent (%d)...\n", current_client);
            bytesRead = read(current_client, buffer, BUFFER_SIZE);
            if (bytesRead <= 0) {
                printf("[DEBUG] Clientul %d s-a deconectat sau nu a trimis date. bytesRead=%ld\n", current_client, bytesRead);
                //write(opponent_client, "exit", 5); // Informăm adversarul
                verificare = true;
                break;
            }
            printf("[DEBUG] Am citit de la client: %s\n", buffer);

            buffer[bytesRead] = '\0';

            // Dacă clientul vrea să părăsească jocul
            if (strcmp(buffer, "exit") == 0) {
                printf("[pair %d] Clientul %d a trimis exit. Conexiunea se închide.\n", pair->idPair, current_client);
                write(opponent_client, "exit", 5); // Informăm adversarul
                break;
            }

            // Parsăm coordonatele mutării
            int num_params = sscanf(buffer, "%d %d %d %d", &from_x, &from_y, &to_x, &to_y);
            printf("ok\n");
            if (num_params != 4) {
                snprintf(buffer, BUFFER_SIZE, "invalid: Mutare invalida! Foloseste formatul x1 y1 x2 y2.");
                write(current_client, buffer, strlen(buffer) + 1);
                continue;
            }

            char piesa = identifica_piesa(board, from_x, from_y);
            if(piesa == 'P') {
                if(validare_miscare_pion(board, from_x, from_y, to_x, to_y, current_color, -1, -1, -1, -1)==true) {
                    break;
                }else {
                    snprintf(buffer, BUFFER_SIZE, "invalid: Mutare invalida! Te rog sa incerci din nou.");
                    write(current_client, buffer, strlen(buffer) + 1); // Trimitem mesajul de eroare
                }
            }else if(piesa == 'T') {
                if(validare_miscare_tura(board, from_x, from_y, to_x, to_y, current_color)==true) {
                    break;
                }else {
                    snprintf(buffer, BUFFER_SIZE, "invalid: Mutare invalida! Te rog sa incerci din nou.");
                    write(current_client, buffer, strlen(buffer) + 1); // Trimitem mesajul de eroare
                }
            }else if(piesa == 'C') {
                if(validare_miscare_cal(board, from_x, from_y, to_x, to_y, current_color)==true) {
                    break;
                }else {
                    snprintf(buffer, BUFFER_SIZE, "invalid: Mutare invalida! Te rog sa incerci din nou.");
                    write(current_client, buffer, strlen(buffer) + 1); // Trimitem mesajul de eroare
                }
            }else if(piesa == 'N') {
                if(validare_miscare_nebun(board, from_x, from_y, to_x, to_y, current_color)==true) {
                    break;
                }else {
                    snprintf(buffer, BUFFER_SIZE, "invalid: Mutare invalida! Te rog sa incerci din nou.");
                    write(current_client, buffer, strlen(buffer) + 1); // Trimitem mesajul de eroare
                }
            }else if(piesa == 'Q') {
                if(validare_miscare_regina(board, from_x, from_y, to_x, to_y, current_color)==true) {
                    break;
                }else {
                    snprintf(buffer, BUFFER_SIZE, "invalid: Mutare invalida! Te rog sa incerci din nou.");
                    write(current_client, buffer, strlen(buffer) + 1); // Trimitem mesajul de eroare
                }
            }else if(piesa == 'K') {
                if(validare_miscare_rege(board, from_x, from_y, to_x, to_y, current_color)==true) {
                    break;
                }else {
                    snprintf(buffer, BUFFER_SIZE, "invalid: Mutare invalida! Te rog sa incerci din nou.");
                    write(current_client, buffer, strlen(buffer) + 1); // Trimitem mesajul de eroare
                }
            }else {
                snprintf(buffer, BUFFER_SIZE, "invalid: Mutare invalida! Te rog sa incerci din nou.");
                write(current_client, buffer, strlen(buffer) + 1); // Trimitem mesajul de eroare
            }

        }

        if (bytesRead <= 0) break;

        // Actualizăm tabla și informăm adversarul
        update_board(board, from_x, from_y, to_x, to_y);
        bzero(buffer, sizeof(buffer));
        strcpy(buffer, "valid");
        write(opponent_client, buffer, strlen(buffer) + 1);

        // Schimbăm rândul
        int temp = current_client;
        current_client = opponent_client;
        opponent_client = temp;
        current_color = (current_color == 'A') ? 'N' : 'A';
        if (current_client == client1) {
            sem_post(&pair->sem_client1);
        } else {
            sem_post(&pair->sem_client2);
        }

        memset(buffer, 0, sizeof(buffer));

    }

    close(client1);
    close(client2);
    free_chess_board(board);
    sem_destroy(&pair->sem_client1);
    sem_destroy(&pair->sem_client2);
}
