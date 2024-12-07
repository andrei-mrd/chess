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
#include <sys/time.h>    // Pentru struct timeval


#define PORT 50000
#define BUFFER_SIZE 1024

typedef struct {
    int client_socket;
    int write_pipe;
} PlayerData;

typedef struct {
    int client1;
    int client2;
    int player1_pipe[2];
    int player2_pipe[2];
    pthread_t player1_thread;
    pthread_t player2_thread;
    pthread_t admin_thread;
} GamePair;

void *handle_player(void *arg);
void *admin_pair(void *arg);
int init_server();

int main() {
    int sd = init_server();
    struct sockaddr_in from;
    socklen_t length = sizeof(from);

    printf("[server] Așteptăm clienți la portul %d...\n", PORT);

    while (1) {
        int client1 = accept(sd, (struct sockaddr *)&from, &length);
        if (client1 < 0) {
            perror("[server] Eroare la accept pentru client1.");
            continue;
        }

        int client2 = accept(sd, (struct sockaddr *)&from, &length);
        if (client2 < 0) {
            perror("[server] Eroare la accept pentru client2.");
            close(client1);
            continue;
        }

        printf("[server] client1 = %d și client2 = %d s-au conectat.\n", client1, client2);

        GamePair *pair = (GamePair *)malloc(sizeof(GamePair));
        pair->client1 = client1;
        pair->client2 = client2;

        if (pipe(pair->player1_pipe) == -1 || pipe(pair->player2_pipe) == -1) {
            perror("[server] Eroare la crearea pipe-urilor.");
            close(client1);
            close(client2);
            free(pair);
            continue;
        }

        pthread_create(&pair->player1_thread, NULL, handle_player, 
                       &(PlayerData){client1, pair->player1_pipe[1]});
        pthread_create(&pair->player2_thread, NULL, handle_player, 
                       &(PlayerData){client2, pair->player2_pipe[1]});
        pthread_create(&pair->admin_thread, NULL, admin_pair, pair);
    }

    close(sd);
    return 0;
}

int init_server() {
    int sd;
    struct sockaddr_in server;

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[server] Eroare la socket().");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("[server] Eroare la bind().");
        exit(EXIT_FAILURE);
    }

    if (listen(sd, 10) == -1) {
        perror("[server] Eroare la listen().");
        exit(EXIT_FAILURE);
    }

    return sd;
}

void *handle_player(void *arg) {
    PlayerData *data = (PlayerData *)arg;
    int client_socket = data->client_socket;
    int write_pipe = data->write_pipe;
    char buffer[BUFFER_SIZE];

    while (1) {
        struct timeval timeout;
        timeout.tv_sec = 10;  // Timeout de 10 secunde
        timeout.tv_usec = 0;

        setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("[server] Client %d s-a deconectat.\n", client_socket);
            break;
        }
        write(write_pipe, buffer, bytes_received);
    }

    close(client_socket);
    return NULL;
}

void *admin_pair(void *arg) {
    GamePair *pair = (GamePair *)arg;
    char color1[] = "alb";
    char color2[] = "negru";
    char buffer[BUFFER_SIZE];
    int result;

    // Trimite culorile către jucători
    send(pair->client1, color1, strlen(color1) + 1, 0);
    send(pair->client2, color2, strlen(color2) + 1, 0);
    printf("[admin] Culorile au fost trimise.\n");

    while (1) {
        // Ascultă mesajele de la jucătorul 1
        memset(buffer, 0, BUFFER_SIZE);
        result = read(pair->player1_pipe[0], buffer, BUFFER_SIZE);

        if (result > 0) {
            if (send(pair->client2, buffer, result, 0) <= 0) {
                printf("[admin] Eroare la trimiterea către client2.\n");
                break;
            }
        } else if (result == 0) {
            printf("[admin] Jucătorul 1 s-a deconectat.\n");
            break;
        }

        // Ascultă mesajele de la jucătorul 2
        memset(buffer, 0, BUFFER_SIZE);
        result = read(pair->player2_pipe[0], buffer, BUFFER_SIZE);

        if (result > 0) {
            if (send(pair->client1, buffer, result, 0) <= 0) {
                printf("[admin] Eroare la trimiterea către client1.\n");
                break;
            }
        } else if (result == 0) {
            printf("[admin] Jucătorul 2 s-a deconectat.\n");
            break;
        }
    }

    // Curățare resurse
    close(pair->player1_pipe[0]);
    close(pair->player1_pipe[1]);
    close(pair->player2_pipe[0]);
    close(pair->player2_pipe[1]);
    free(pair);
    return NULL;
}
