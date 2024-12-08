#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

#define PORT 50000
#define BUFFER_SIZE 1024

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

void handle_game(int client1, int client2) {
    int player1_pipe[2], player2_pipe[2];
    pipe(player1_pipe);
    pipe(player2_pipe);

    // Trimite culorile către clienți
    const char *color1 = "alb";
    const char *color2 = "negru";
    send(client1, color1, strlen(color1) + 1, 0);  // Trimite "alb" la clientul 1
    send(client2, color2, strlen(color2) + 1, 0);  // Trimite "negru" la clientul 2
    printf("[server] Culorile au fost trimise: alb -> client1, negru -> client2.\n");

    char buffer[BUFFER_SIZE];
    int result;

    pid_t id = fork();

    if(id < 0) {
        //eroare creere proces
        perror("Eroare la fork()");
        exit(1);
    }else if(id != 0) {
        //clientul 1
        memset(buffer, 0, BUFFER_SIZE);
        result = recv(client1, buffer, BUFFER_SIZE, 0);
        if(result < 0) {
            perror("Eroare primire mesaj de la client1 \n");
            exit(0);
        }
        buffer[result] = '\n';
        printf("Mesajul primit de la client1 este %s\n", buffer);
        if(write(player1_pipe[1], buffer, BUFFER_SIZE) == -1) {
            perror("Eroare scriere in player1_pipe\n");
            exit(0);
        }
        sleep(4);

        //o perioada de sleep
    }else {
        //clientul2
        sleep(2);
        memset(buffer, 0, BUFFER_SIZE);
        int r = read(player1_pipe[0], buffer, BUFFER_SIZE);
        if(r < 0) {
            perror("Eroare la citire din player1_pipe\n");
            exit(0);
        }
        buffer[r] = '\0';
        if(send(client2, buffer, BUFFER_SIZE, 0) == -1) {
            perror("Eroare transmitere mutare de la client1 la client2\n");
            exit(0);
        }
        memset(buffer, 0, BUFFER_SIZE);
        
    }
}

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

        // Începe un joc între cei doi clienți
        handle_game(client1, client2);
    }

    close(sd);
    return 0;
}
