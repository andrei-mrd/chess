#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "chess.h"
#include "interfata_grafica.h"

#define PORT 50000
#define BUFFER_SIZE 1024

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define SQUARE_SIZE (WINDOW_WIDTH / 8)

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

    int** board = create_chess_board();
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Eroare la inițializarea SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Eroare la inițializarea SDL_Image: %s\n", IMG_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow(buffer,
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *pion_alb = IMG_LoadTexture(renderer, "assets/pion_alb.png");
    SDL_Texture *pion_negru = IMG_LoadTexture(renderer, "assets/pion_negru.png");
    SDL_Texture *tura_alba = IMG_LoadTexture(renderer, "assets/tura_alba.png");
    SDL_Texture *tura_neagra = IMG_LoadTexture(renderer, "assets/tura_neagra.png");
    SDL_Texture *nebun_alb = IMG_LoadTexture(renderer, "assets/nebun_alb.png");
    SDL_Texture *nebun_negru = IMG_LoadTexture(renderer, "assets/nebun_negru.png");
    SDL_Texture *cal_alb = IMG_LoadTexture(renderer, "assets/cal_alb.png");
    SDL_Texture *cal_negru = IMG_LoadTexture(renderer, "assets/cal_negru.png");
    SDL_Texture *regina_alba = IMG_LoadTexture(renderer, "assets/regina_alba.png");
    SDL_Texture *regina_neagra = IMG_LoadTexture(renderer, "assets/regina_neagra.png");
    SDL_Texture *rege_alb = IMG_LoadTexture(renderer, "assets/rege_alb.png");
    SDL_Texture *rege_negru = IMG_LoadTexture(renderer, "assets/rege_negru.png");

    SDL_Texture *textures[] = 
    {pion_alb, pion_negru, tura_alba, tura_neagra, nebun_alb, nebun_negru, cal_alb, cal_negru,
        regina_alba, regina_neagra, rege_alb, rege_negru};

    if (!window) {
        printf("Eroare la crearea ferestrei: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if (!renderer) {
        printf("Eroare la crearea renderer-ului: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return EXIT_FAILURE;
    }

    SDL_Event event;

    for (int i = 0; i < 12; i++) {
        if (!textures[i]) {
            printf("Eroare texturi: %s\n", IMG_GetError());
            exit(EXIT_FAILURE);
        }
    }

    display_board(renderer, board, textures);

    bool running = true;
    int from_x = -1, from_y = -1, to_x = -1, to_y = -1, board_x, board_y;

    // Așteaptă mesaje de la server și trimite-le înapoi
    bool esteAlb = (buffer[0] == 'A');
    while (running) {
        if (esteAlb) {
            printf("Introdu mutarea\n");
            from_x = from_y = to_x = to_y = -1;
            if(SDL_WaitEvent(&event) == false) {
                printf("[1] event indentificat\n");
                if(event.type == SDL_QUIT) {
                    break;
                }
                if(event.type == SDL_MOUSEBUTTONDOWN) {
                    board_x = event.button.x;
                    board_y = event.button.y;
                    from_x = board_x / SQUARE_SIZE;
                    from_y = board_y / SQUARE_SIZE;

                    printf("Prima selectie e (%d %d)\n", from_x, from_y);
                }
            }

            if(SDL_WaitEvent(&event) == false) {
                printf("[1] event indentificat\n");
                if(event.type == SDL_QUIT) {
                    break;
                }
                if(event.type == SDL_MOUSEBUTTONDOWN) {
                    board_x = event.button.x;
                    board_y = event.button.y;
                    to_x = board_x / SQUARE_SIZE;
                    to_y = board_y / SQUARE_SIZE;

                    printf("A 2 a selectie e (%d %d)\n", to_x, to_y);
                }
            }

            //Trimitem mutarea
            memset(buffer, 0, BUFFER_SIZE);
            snprintf(buffer, BUFFER_SIZE, "%d %d %d %d", from_x, from_y, to_x, to_y);
            result = send(client_socket, buffer, strlen(buffer), 0);
            if (result <= 0) {
                printf("Eroare la trimiterea mutării.\n");
                break;
            }
            printf("Mutarea ta a fost trimisă.\n");
            // Așteaptă răspunsul de la server
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
            printf("Introdu mutarea\n");
            from_x = from_y = to_x = to_y = -1;
            if(SDL_WaitEvent(&event) == false) {
                printf("[1] event indentificat\n");
                if(event.type == SDL_QUIT) {
                    break;
                }
                if(event.type == SDL_MOUSEBUTTONDOWN) {
                    board_x = event.button.x;
                    board_y = event.button.y;
                    from_x = board_x / SQUARE_SIZE;
                    from_y = board_y / SQUARE_SIZE;

                    printf("Prima selectie e (%d %d)\n", from_x, from_y);
                }
            }

            if(SDL_WaitEvent(&event) == false) {
                printf("[1] event indentificat\n");
                if(event.type == SDL_QUIT) {
                    break;
                }
                if(event.type == SDL_MOUSEBUTTONDOWN) {
                    board_x = event.button.x;
                    board_y = event.button.y;
                    to_x = board_x / SQUARE_SIZE;
                    to_y = board_y / SQUARE_SIZE;

                    printf("A 2 a selectie e (%d %d)\n", to_x, to_y);
                }
            }

            //Trimitem mutarea
            memset(buffer, 0, BUFFER_SIZE);
            snprintf(buffer, BUFFER_SIZE, "%d %d %d %d", from_x, from_y, to_x, to_y);
            result = send(client_socket, buffer, strlen(buffer), 0);
            if (result <= 0) {
                printf("Eroare la trimiterea mutării.\n");
                break;
            }
            printf("Mutarea ta a fost trimisă.\n");
        }
    }

    for (int i = 0; i < 12; i++) {
        if (textures[i] != NULL) {
            SDL_DestroyTexture(textures[i]);
            textures[i] = NULL;
        }
    }

    free_chess_board(board);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
    close(client_socket);
    return 0;
}
