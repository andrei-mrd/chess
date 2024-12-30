#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "chess.h"

#define PORT 50000
#define BUFFER_SIZE 1024

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define SQUARE_SIZE (WINDOW_WIDTH / 8)

void draw_board(SDL_Renderer *renderer) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            SDL_Rect square = {col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
            if ((row + col) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//alb
            } else {
                SDL_SetRenderDrawColor(renderer, 118, 150, 86, 255);//verde
            }
            SDL_RenderFillRect(renderer, &square);
        }
    }
}

void get_move_from_mouse(int *row, int *col) {
    SDL_Event event;
    bool waiting = true;

    while (waiting) {
        while (SDL_WaitEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                *col = event.button.x / SQUARE_SIZE;
                *row = event.button.y / SQUARE_SIZE;
                waiting = false;
                break;
            }
        }
    }
}

void draw_piece(SDL_Renderer *renderer, SDL_Texture *piece, int row, int col) {
    SDL_Rect dest = {col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
    SDL_RenderCopy(renderer, piece, NULL, &dest);
}

void display_board(SDL_Renderer *renderer, SDL_Texture *textures[], int **board) {
    bool running = true;
    SDL_Event event;

    while(running) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        draw_board(renderer);

        for(int i = 0; i<8; i++) {
            for(int j = 0; j<8 ;j++) {
                if(board[j][i] == -1) {
                    draw_piece(renderer, textures[0], j, i);
                }else if(board[j][i] == 1) {
                    draw_piece(renderer, textures[1], j, i);
                }else if(board[j][i] == -2) {
                    draw_piece(renderer, textures[4], j, i);
                }else if(board[j][i] == 2) {
                    draw_piece(renderer, textures[5], j, i);
                }else if(board[j][i] == -3) {
                    draw_piece(renderer, textures[6], j, i);
                }else if(board[j][i] == 3) {
                    draw_piece(renderer, textures[7], j, i);
                }else if(board[j][i] == -4) {
                    draw_piece(renderer, textures[2], j, i);
                }else if(board[j][i] == 4) {
                    draw_piece(renderer, textures[3], j, i);
                }else if(board[j][i] == -5) {
                    draw_piece(renderer, textures[8], j, i);
                }else if(board[j][i] == 5) {
                    draw_piece(renderer, textures[9], j, i);
                }else if(board[j][i] == -6) {
                    draw_piece(renderer, textures[10], j, i);
                }else if(board[j][i] == 6) {
                    draw_piece(renderer, textures[11], j, i);
                }
            }
        }
        SDL_RenderPresent(renderer);
        return;
    }
}

void show_board(int **board, SDL_Renderer *renderer, SDL_Texture *textures[]) {
    draw_board(renderer);
    for(int i = 0; i<8; i++) {
        for(int j = 0; j<8 ;j++) {
            if(board[j][i] == -1) {
                draw_piece(renderer, textures[0], j, i);
            }else if(board[j][i] == 1) {
                draw_piece(renderer, textures[1], j, i);
            }else if(board[j][i] == -2) {
                draw_piece(renderer, textures[4], j, i);
            }else if(board[j][i] == 2) {
                draw_piece(renderer, textures[5], j, i);
            }else if(board[j][i] == -3) {
                draw_piece(renderer, textures[6], j, i);
            }else if(board[j][i] == 3) {
                draw_piece(renderer, textures[7], j, i);
            }else if(board[j][i] == -4) {
                draw_piece(renderer, textures[2], j, i);
            }else if(board[j][i] == 4) {
                draw_piece(renderer, textures[3], j, i);
            }else if(board[j][i] == -5) {
                draw_piece(renderer, textures[8], j, i);
            }else if(board[j][i] == 5) {
                draw_piece(renderer, textures[9], j, i);
            }else if(board[j][i] == -6) {
                draw_piece(renderer, textures[10], j, i);
            }else if(board[j][i] == 6) {
                draw_piece(renderer, textures[11], j, i);
            }
        }
    }
    SDL_RenderPresent(renderer);
    return;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Creare socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Eroare la socket().");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Conectare la server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Eroare la connect().");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Primește culoarea (alb sau negru)
    memset(buffer, 0, BUFFER_SIZE);
    int result = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (result <= 0) {
        printf("Eroare la primirea culorii. Conexiunea a fost închisă.");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    bool esteAlb = (buffer[0] == 'A');
    printf("Culoarea ta este: %s\n", esteAlb ? "Alb" : "Negru");

    // Inițializare SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Eroare la initializare SDL: %s\n", SDL_GetError());
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    const char *window_title = esteAlb ? "Tabla de sah - Alb" : "Tabla de sah - Negru";

    SDL_Window *window = SDL_CreateWindow(window_title,
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        fprintf(stderr, "Eroare la crearea ferestrei: %s\n", SDL_GetError());
        SDL_Quit();
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Eroare la crearea rendererului: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        close(client_socket);
        exit(EXIT_FAILURE);
    }

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

    int ** board = create_chess_board();

    bool is_valid_move;
    bool running = true;
    int from_x, from_y, to_x,to_y, board_x, board_y;

    display_board(renderer, textures, board);

    while (running) {
        if (esteAlb) {
            printf("Introdu mutarea\n");
            from_x = from_y = to_x = to_y = -1;
            int selectie = 0;

            // Așteaptă selecția piesei și a destinației
            bool is_break = false;

            while (selectie < 2) {
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        running = false;
                        is_break = true;
                        break;
                    }
                    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                        int col = event.button.x / SQUARE_SIZE;
                        int row = event.button.y / SQUARE_SIZE;

                        if (selectie == 0) {
                            from_x = col;
                            from_y = row;
                            printf("Prima selecție: (%d, %d)\n", from_x, from_y);
                            if(selectare_valida_1(board, from_x, from_y, 'A') == false) {
                                printf("Selectie invalida pt prima mutare\n");
                                continue;
                            }else selectie++;
                        } else if (selectie == 1) {
                            to_x = col;
                            to_y = row;
                            printf("A doua selecție: (%d, %d)\n", to_x, to_y);
                            if(selectare_valida_2(board, from_x, from_y, to_x, to_y, 'A') == false) {
                                printf("Selectie invalida pt a 2 mutare\n");
                                selectie--;
                                continue;
                            }else {
                                selectie++;
                            }
                        }
                    }
                }
                if(is_break == true) {
                    break;
                }
            }

            if(is_break == true) {
                break;
            }

            update_board_memory(board, from_x, from_y, to_x, to_y);

            show_board(board, renderer, textures);

            // Trimitem mutarea
            memset(buffer, 0, BUFFER_SIZE);
            snprintf(buffer, BUFFER_SIZE, "%d %d %d %d", from_x, from_y, to_x, to_y);
            result = send(client_socket, buffer, strlen(buffer), 0);
            if (result <= 0) {
                printf("Eroare la trimiterea mutării.\n");
                running = false;
                break;
            }
            printf("Mutarea ta a fost trimisă.\n");

            // Așteaptă răspunsul de la server
            memset(buffer, 0, BUFFER_SIZE);
            result = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (result <= 0) {
                printf("Eroare la primirea răspunsului.\n");
                running = false;
                break;
            }
            printf("Răspuns de la server: %s\n", buffer);

            sscanf(buffer, "%d %d %d %d", &from_x, &from_y, &to_x, &to_y);

            update_board_memory(board, from_x, from_y, to_x, to_y);
            show_board(board, renderer, textures);

        } else {
            // Așteaptă mutare de la clientul alb
            memset(buffer, 0, BUFFER_SIZE);
            result = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (result < 0) {
                printf("Eroare la primirea mutării.\n");
                running = false;
                break;
            }
            buffer[result] = '\0';
            printf("Mutare primită: %s\n", buffer);

            sscanf(buffer,"%d %d %d %d", &from_x, &from_y, &to_x, &to_y);

            update_board_memory(board, from_x, from_y, to_x, to_y);
            show_board(board, renderer, textures);

            from_x = from_y = to_x = to_y = -1;
            int selectie = 0;

            bool is_break = false;

            while (selectie < 2) {
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        running = false;
                        is_break = true;
                        break;
                    }
                    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                        int col = event.button.x / SQUARE_SIZE;
                        int row = event.button.y / SQUARE_SIZE;

                        if (selectie == 0) {
                            from_x = col;
                            from_y = row;
                            printf("Prima selecție: (%d, %d)\n", from_x, from_y);
                            if(selectare_valida_1(board, from_x, from_y, 'N') == false) {
                                printf("Selectie invalida pt prima mutare\n");
                                continue;
                            }else selectie++;
                        } else if (selectie == 1) {
                            to_x = col;
                            to_y = row;
                            printf("A doua selecție: (%d, %d)\n", to_x, to_y);
                            if(selectare_valida_2(board, from_x, from_y, to_x, to_y, 'N') == false) {
                                printf("Selectie invalida pt a 2 a mutare\n");
                                selectie--;
                                continue;
                            }else {
                                selectie++;
                            }
                        }
                    }
                }
                if(is_break == true) {
                    break;
                }
            }

            if(is_break == true) {
                break;
            }

            update_board_memory(board, from_x, from_y, to_x, to_y);

            show_board(board, renderer, textures);

            // Trimitem mutarea
            memset(buffer, 0, BUFFER_SIZE);
            snprintf(buffer, BUFFER_SIZE, "%d %d %d %d", from_x, from_y, to_x, to_y);
            result = send(client_socket, buffer, strlen(buffer), 0);
            if (result < 0) {
                printf("Eroare la trimiterea mutării.\n");
                running = false;
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
    close(client_socket);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
