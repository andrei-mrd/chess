#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 50000
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Creăm socketul
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[client] Eroare la crearea socketului.");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Conectare la server
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("[client] Eroare la conexiunea la server.");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[client] Eroare la connect().");
        exit(EXIT_FAILURE);
    }

    // Primește culoarea de la server
    if (recv(sockfd, buffer, BUFFER_SIZE, 0) <= 0) {
        printf("[client] Eroare la primirea culorii de la server.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Afișează culoarea primită
    printf("[client] Ai primit culoarea: %s\n", buffer);

    if(buffer[0] == 'a') {
        while(1) {
            // Așteaptă input-ul jucătorului pentru mișcare
            printf("Introdu mișcarea ta (ex: e2 e4): ");
            memset(buffer, 0, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';  // Elimină newline-ul de la final

            if (send(sockfd, buffer, strlen(buffer), 0) <= 0) {
                printf("[client] Eroare la trimiterea mișcării.\n");
                break;
            }

            // Așteaptă răspunsul de la server (mișcarea celuilalt jucător)
            memset(buffer, 0, BUFFER_SIZE);
            if (recv(sockfd, buffer, BUFFER_SIZE, 0) <= 0) {
                printf("[client] Eroare la primirea răspunsului de la server.\n");
                break;
            }

            // Afișează mișcarea primit de la celălalt jucător
            printf("[client] Mișcarea adversarului: %s\n", buffer);
        }
    }else if(buffer[0] == 'n') {
        while(1) {
            sleep(1);
            memset(buffer, 0, BUFFER_SIZE);
            if (recv(sockfd, buffer, BUFFER_SIZE, 0) <= 0) {
                printf("[client] Eroare la primirea răspunsului de la server.\n");
                break;
            }

            printf("[client] Mișcarea adversarului: %s\n", buffer);
            
            // Așteaptă input-ul jucătorului pentru mișcare
            printf("Introdu mișcarea ta (ex: e2 e4): ");
            memset(buffer, 0, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';  // Elimină newline-ul de la final

            if (send(sockfd, buffer, strlen(buffer), 0) <= 0) {
                printf("[client] Eroare la trimiterea mișcării.\n");
                break;
            }

        }
    }


    /*
    while (1) {

        // Așteaptă input-ul jucătorului pentru mișcare
        printf("Introdu mișcarea ta (ex: e2 e4): ");
        memset(buffer, 0, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';  // Elimină newline-ul de la final

        if (send(sockfd, buffer, strlen(buffer), 0) <= 0) {
            printf("[client] Eroare la trimiterea mișcării.\n");
            break;
        }

        // Așteaptă răspunsul de la server (mișcarea celuilalt jucător)
        memset(buffer, 0, BUFFER_SIZE);
        if (recv(sockfd, buffer, BUFFER_SIZE, 0) <= 0) {
            printf("[client] Eroare la primirea răspunsului de la server.\n");
            break;
        }

        // Afișează mișcarea primit de la celălalt jucător
        printf("[client] Mișcarea adversarului: %s\n", buffer);
    }
    */

    // Închide conexiunea
    close(sockfd);
    return 0;
}
