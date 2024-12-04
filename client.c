#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>  // Pentru fd_set, FD_* macros și select()
#include <unistd.h>      // Pentru STDIN_FILENO, read(), write()
#include <sys/time.h>    // Pentru struct timeval

/* codul de eroare returnat de anumite apeluri */
extern int errno;

#define BUFFER_SIZE 1024

/* portul de conectare la server */
int port;

int main(int argc, char *argv[]) {
    int sd;                         // descriptorul de socket
    struct sockaddr_in server;      // structura folosita pentru conectare 
    int nr = 0;                     // mesajul trimis
    char buffer[1024];
    char buf[1024];                   // buffer pentru citirea mesajului

    /* verificarea argumentelor */
    if (argc != 3) {
        fprintf(stderr, "Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    /* stabilim portul */
    port = atoi(argv[2]);

    /* crearea socketului */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Eroare la socket().\n");
        return errno;
    }

    /* configurarea structurii pentru conectare */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);

    /* conectarea la server */
    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
        perror("[client]Eroare la connect().\n");
        close(sd);
        return errno;
    }

    int flags = fcntl(sd, F_GETFL, 0);
    fcntl(sd, F_SETFL, flags | O_NONBLOCK);

    while (1) {
        fd_set read_fds;
        struct timeval timeout;

        FD_ZERO(&read_fds);
        FD_SET(sd, &read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        timeout.tv_sec = 1;  // Timeout de 1 secundă
        timeout.tv_usec = 0;

        int activity = select(sd + 1, &read_fds, NULL, NULL, &timeout);

        if (activity < 0 && errno != EINTR) {
            perror("[client] Eroare la select()");
            break;
        }

        // Verificăm dacă serverul a trimis un mesaj
        if (FD_ISSET(sd, &read_fds)) {
            bzero(buffer, sizeof(buffer));
            if (read(sd, buffer, BUFFER_SIZE) <= 0) {
                printf("[client] Conexiunea cu serverul a fost pierdută.\n");
                break;
            }

            if (strcmp(buffer, "exit") == 0) {
                printf("[client] Serverul a trimis comanda de închidere. Conexiunea se termină.\n");
                break;
            }

            printf("[client] Mesajul primit de la server: %s\n", buffer);
        }

        // Verificăm dacă utilizatorul introduce un mesaj
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                perror("[client] Eroare la citirea inputului.\n");
                break;
            }

            buf[strcspn(buf, "\n")] = 0;  // Eliminăm newline-ul
            if (write(sd, buf, strlen(buf) + 1) <= 0) {
                perror("[client] Eroare la scriere către server.\n");
                break;
            }

            if (strcmp(buf, "exit") == 0) {
                printf("[client] Trimitem comanda de ieșire la server. Conexiunea se închide.\n");
                break;
            }
        }
    }


    /* inchiderea conexiunii */
    close(sd);
    return 0;
}
