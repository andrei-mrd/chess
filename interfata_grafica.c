#include "chess.h"
#include "interfata_grafica.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

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

void draw_piece(SDL_Renderer *renderer, SDL_Texture *piece, int row, int col) {
    SDL_Rect dest = {col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
    SDL_RenderCopy(renderer, piece, NULL, &dest);
}

void display_board(SDL_Renderer *renderer, int**  table, SDL_Texture *textures[]) {
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
                if(table[i][j] == -1) {
                    draw_piece(renderer, textures[0], i, j);
                }else if(table[i][j] == 1) {
                    draw_piece(renderer, textures[1], i, j);
                }else if(table[i][j] == -2) {
                    draw_piece(renderer, textures[4], i, j);
                }else if(table[i][j] == 2) {
                    draw_piece(renderer, textures[5], i, j);
                }else if(table[i][j] == -3) {
                    draw_piece(renderer, textures[6], i, j);
                }else if(table[i][j] == 3) {
                    draw_piece(renderer, textures[7], i, j);
                }else if(table[i][j] == -4) {
                    draw_piece(renderer, textures[2], i, j);
                }else if(table[i][j] == 4) {
                    draw_piece(renderer, textures[3], i, j);
                }else if(table[i][j] == -5) {
                    draw_piece(renderer, textures[8], i, j);
                }else if(table[i][j] == 5) {
                    draw_piece(renderer, textures[9], i, j);
                }else if(table[i][j] == -6) {
                    draw_piece(renderer, textures[10], i, j);
                }else if(table[i][j] == 6) {
                    draw_piece(renderer, textures[11], i, j);
                }
            }
        }
        SDL_RenderPresent(renderer);
        return;
    }
}


/*
void update_board_graphic(SDL_Renderer *renderer, int** table, SDL_Texture *textures[]) {
    printf("Waiting for mouse input...\n");
    bool waiting = true;
    SDL_Event event;

    int selected_x = -1, selected_y = -1; // Piesa selectată
    int to_x, to_y;

    // Dimensiunea tablei în pixeli
    int board_size = 800; // Exemplu: tabla este de 600x600 pixeli
    int cell_size = board_size / 8; // Fiecare celulă este 75x75 pixeli

    while (waiting) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                waiting = false;
                return; // Ieşire
            }

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                // Transformă coordonatele mouse-ului în coordonate logice
                int board_x = mouse_x / cell_size;
                int board_y = mouse_y / cell_size;

                if (selected_x == -1 && selected_y == -1) {
                    // Selectează piesa
                    selected_x = board_x;
                    selected_y = board_y;
                    printf("selected_x : %d\n", selected_x);
                    printf("selected_y : %d\n", selected_y);
                } else {
                    // Mutare piesă
                    to_x = board_x;
                    to_y = board_y;
                    printf("to_x : %d\n", to_x);
                    printf("to_y : %d\n", to_y);
                    waiting = false;
                    return;
                }
            }
        }
    }
}

*/
void play_game(SDL_Renderer *renderer, int** table, SDL_Texture *textures[], char culoare, int sd) {
    bool running = true;
    SDL_Event event;

    int board_size = 800;//tabla e de 800x800
    int cell_size = 100;//dimensiunea fiecarui patrat e de 100 pe 100
    int selected_x = -1, selected_y = -1, to_x = -1, to_y = -1;
    bool waiting = true;

    display_board(renderer, table, textures);

    while(waiting) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
                return;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                // Transformă coordonatele mouse-ului în coordonate logice
                int board_x = mouse_x / cell_size;
                int board_y = mouse_y / cell_size;

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
                    char c = identifica_piesa(table, selected_x, selected_y);
                    switch (c)
                    {
                    case 'P':
                        if(table[selected_y][selected_x] == -1) {
                            if(validare_miscare_pion(table, selected_x, selected_y, to_x, to_y, 'A', -1, -1, -1, -1) == true) {
                                update_board(table, selected_x, selected_y, to_x, to_y);
                                display_board(renderer, table, textures);
                            }
                        }else if(table[selected_y][selected_x] == 1) {
                            if(validare_miscare_pion(table, selected_x, selected_y, to_x, to_y, 'N', -1, -1, -1, -1) == true) {
                                update_board(table, selected_x, selected_y, to_x, to_y);
                                display_board(renderer, table, textures);
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
}
