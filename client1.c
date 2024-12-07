#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "interfata_grafica.h"
#include "chess.h"

#define SERVER_IP "127.0.0.1"  // Adresa IP a serverului
#define SERVER_PORT 50000      // Portul serverului
#define BUFFER_SIZE 1024       // Dimensiunea buffer-ului de citire/scriere

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define SQUARE_SIZE (WINDOW_WIDTH / 8)

void handle_game(int socket);

int main() {
    int sock;
    struct sockaddr_in server_addr;

    // Crearea socket-ului
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Eroare la crearea socket-ului");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    // Conversia IP-ului în format binar
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Eroare la conversia IP-ului");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Conectarea la server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Eroare la conectare");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("[client] Conectat la server\n");

    // Tratarea jocului
    handle_game(sock);

    close(sock);
    return 0;
}

void handle_game(int socket) {
    char buffer[BUFFER_SIZE];
    char color;

    // Primirea culorii piesei
    ssize_t bytesRead = read(socket, &color, 1);
    if (bytesRead <= 0) {
        perror("[client] Eroare la citirea culorii");
        close(socket);
        return;
    }

    // Afișăm culoarea jucătorului
    if (color == 'A') {
        printf("[client] Ești jucătorul cu piesele albe (A)\n");
    } else if (color == 'N') {
        printf("[client] Ești jucătorul cu piesele negre (N)\n");
    } else {
        printf("[client] Culoare invalidă primită\n");
        close(socket);
        return;
    }

    int** board = create_chess_board();
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    char title[6] = "Board";
    if(color == 'A') {
        strcat(title, "A");
    } else {
        strcat(title, "N");
    }

    SDL_Window *window = SDL_CreateWindow(title,
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

    for(int i = 0; i<12; i++) {
        if(!textures[i]) {
            printf("Eroare texturi: %s\n", IMG_GetError());
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < 12; i++) {
        if(!textures[i]) {
            printf("Eroare texturi: %s\n", IMG_GetError());
            exit(EXIT_FAILURE);
        }
    }

    display_board(renderer, board, textures);

    // Jocul propriu-zis
    SDL_Event event;
    int selected_x1 = -1, selected_y1 = -1;
    int selected_x2 = -1, selected_y2 = -1;

    bool verificare;
    int count = 0;

    int from_x, from_y, to_x, to_y;

    while (1) {
        from_x = from_y = to_x = to_y = -1;
        
        ssize_t bytesRead = read(socket, buffer, BUFFER_SIZE);
        buffer[bytesRead] = '\0';
        int num_params = sscanf(buffer, "%d %d %d %d", &from_x, &from_y, &to_x, &to_y);
        if(from_x != -1) {
            update_board(board, from_x, from_y, to_x, to_y);
            display_board(renderer, board, textures);
        }
        memset(buffer, 0, sizeof(buffer));

        verificare = false;
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                write(socket, "exit", 5);
                printf("[client] Ai părăsit jocul.\n");
                verificare = true;
                break;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                int x = mouse_x / SQUARE_SIZE;
                int y = mouse_y / SQUARE_SIZE;

                if (selected_x1 == -1) {
                    selected_x1 = x;
                    selected_y1 = y;
                    printf("[client] Ai selectat prima poziție: (%d, %d)\n", selected_x1, selected_y1);
                } else {
                    selected_x2 = x;
                    selected_y2 = y;
                    printf("[client] Ai selectat a doua poziție: (%d, %d)\n", selected_x2, selected_y2);

                    snprintf(buffer, sizeof(buffer), "%d %d %d %d", selected_x1, selected_y1, selected_x2, selected_y2);
                    write(socket, buffer, strlen(buffer) + 1);


                    // Citim răspunsul de la server
                    ssize_t bytesRead = read(socket, buffer, BUFFER_SIZE);
                    if (bytesRead <= 0) {
                        perror("[client] Eroare la citirea răspunsului serverului");
                        break;
                    }

                    buffer[bytesRead] = '\0';

                    if (strcmp(buffer, "valid") == 0) {
                        printf("[client] Mutare validă.\n");
                        // Actualizăm tabla și redepictăm
                        update_board(board, selected_x1, selected_y1, selected_x2, selected_y2);
                        display_board(renderer, board, textures);
                        // Resetăm selecțiile
                        selected_x1 = -1;
                        selected_y1 = -1;
                        selected_x2 = -1;
                        selected_y2 = -1;
                        break;
                    } else {
                        printf("[client] Mutare invalidă: %s\n", buffer);
                        // Resetăm selecțiile
                        selected_x1 = -1;
                        selected_y1 = -1;
                        selected_x2 = -1;
                        selected_y2 = -1;
                    }
                }
            }
        }

        // Verificăm dacă jocul s-a încheiat
        if (verificare) {
            break;
        }
        count++;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
}
