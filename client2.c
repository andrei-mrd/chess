#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 50000
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int result;

    // Creare socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Eroare la socket().");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Conectează-te la orice adresă IP

    // Conectare la server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Eroare la connect().");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Primește culoarea (alb sau negru)
    memset(buffer, 0, BUFFER_SIZE);
    result = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (result <= 0) {
        printf("Eroare la primirea culorii.\n");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Culoarea ta este: %s\n", buffer);

    // Așteaptă mesaje de la server și trimite-le înapoi
    while (1) {
        if(buffer[0] == 'a') {
            // Trimite mesajul înapoi la server
            memset(buffer, 0, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE, stdin);
            result = send(client_socket, buffer, strlen(buffer), 0);
            if (result <= 0) {
                printf("Eroare la trimiterea mesajului către server.\n");
                break;
            }
            memset(buffer, 0, BUFFER_SIZE);
            result = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (result <= 0) {
                printf("Serverul s-a deconectat sau a apărut o eroare.\n");
                break;
            }
            buffer[result] = '\0';
            printf("Mesaj de la server: %s\n", buffer);
        }else if(buffer[0] == 'n') {
            memset(buffer, 0, BUFFER_SIZE);
            result = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (result <= 0) {
                printf("Serverul s-a deconectat sau a apărut o eroare.\n");
                break;
            }
            buffer[result] = '\0';
            printf("Mesaj de la server: %s\n", buffer);

            memset(buffer, 0, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE, stdin);
            result = send(client_socket, buffer, strlen(buffer), 0);
            if (result <= 0) {
                printf("Eroare la trimiterea mesajului către server.\n");
                break;
            }
        }
    }

    close(client_socket);
    return 0;
}
