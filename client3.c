#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

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
    bool esteAlb = (buffer[0] == 'A');
    while (1) {
        if (esteAlb) {
            // Trimite mutare
            memset(buffer, 0, BUFFER_SIZE);
            printf("Introdu mutarea ta: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            result = send(client_socket, buffer, strlen(buffer), 0);
            if (result <= 0) {
                printf("Eroare la trimiterea mutării.\n");
                break;
            }
            printf("Mutarea ta a fost trimisă.\n");

            // Așteaptă răspuns
            memset(buffer, 0, BUFFER_SIZE);
            result = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (result <= 0) {
                printf("Eroare la primirea răspunsului.\n");
                break;
            }
            printf("Răspuns de la server: %s\n", buffer);
        } else {
            // Așteaptă mutare de la clientul 1
            memset(buffer, 0, BUFFER_SIZE);
            result = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (result <= 0) {
                printf("Eroare la primirea mutării.\n");
                break;
            }
            buffer[result] = '\0';
            printf("Mutare primită: %s\n", buffer);

            // Trimite răspunsul
            memset(buffer, 0, BUFFER_SIZE);
            printf("Introdu răspunsul: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            result = send(client_socket, buffer, strlen(buffer), 0);
            if (result <= 0) {
                printf("Eroare la trimiterea răspunsului.\n");
                break;
            }
            printf("Mutarea ta a fost trimisă.\n");
        }
    }



    close(client_socket);
    return 0;
}
