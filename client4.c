#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "chess.h"
#include "interfata_grafica.h"

#define SERVER_IP "127.0.0.1"  // Adresa IP a serverului
#define SERVER_PORT 50000      // Portul serverului
#define BUFFER_SIZE 1024       // Dimensiunea buffer-ului de citire/scriere

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define SQUARE_SIZE (WINDOW_WIDTH / 8)

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

    char buffer;

    int result = recv(sock, buffer, 1, 0);
    if(result <= 0) {
        perror("Eroare primire culoare de la server\n");
    }

    if(buffer == 'A') {
        printf("Ai primit culoarea alba\n");
    }else {
        printf("Ai primit culoarea neagra\n");
    }

    printf("[client] Conectat la server\n");

    int** board = create_chess_board();
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);


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

    for(int i = 0; i<12; i++) {
        if(!textures[i]) {
            printf("Eroare texturi: %s\n", IMG_GetError());
            exit(EXIT_FAILURE);
        }
    }

    SDL_Event event;

    int selected_x = -1, selected_y = -1, to_x = -1, to_y = -1;
    bool running = true;
    bool waiting = true;

    display_board(renderer, board, textures);

    while(running) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
                break;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                // Transformă coordonatele mouse-ului în coordonate logice
                int board_x = mouse_x / SQUARE_SIZE;
                int board_y = mouse_y / SQUARE_SIZE;

                if (selected_x == -1 && selected_y == -1) {
                    //selecteaza piesa
                    selected_x = board_x;
                    selected_y = board_y;
                    printf("selected_x : %d\n", selected_x);
                    printf("selected_y : %d\n", selected_y);
                    //verificam daca ce am selectat e o piesa valida
                } else {
                    //muta piesa
                    to_x = board_x;
                    to_y = board_y;
                    char c = identifica_piesa(board, selected_x, selected_y);
                    switch (c)
                    {
                    case 'P':
                        if(board[selected_y][selected_x] == -1) {
                            if(validare_miscare_pion(board, selected_x, selected_y, to_x, to_y, 'A', -1, -1, -1, -1) == true) {
                                update_board(board, selected_x, selected_y, to_x, to_y);
                                display_board(renderer, board, textures);
                            }
                        }else if(board[selected_y][selected_x] == 1) {
                            if(validare_miscare_pion(board, selected_x, selected_y, to_x, to_y, 'N', -1, -1, -1, -1) == true) {
                                update_board(board, selected_x, selected_y, to_x, to_y);
                                display_board(renderer, board, textures);
                            }
                        }
                        break;
                    
                    default:
                        break;
                    }
                    printf("to_x : %d\n", to_x);
                    printf("to_y : %d\n", to_y);
                    selected_x = -1;
                    selected_y = -1;
                }
            }
        }
    }

    return 0;
}
