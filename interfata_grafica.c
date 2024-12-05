#include "chess.h"
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
void play_game(SDL_Renderer *renderer, int** table, SDL_Texture *textures[]) {
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
                    if(table[selected_y][selected_x] != 7 && table[selected_y][selected_x] != 8) {
                        printf("Ai selectat o piesa valida\n");
                    }else {
                        selected_x = -1;
                        selected_y = -1;
                        printf("Ai selectat o piesa invalida\n");
                    }
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


int main() {
    int** board = create_chess_board();
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG); // Inițializează suportul pentru imagini PNG

    SDL_Window *window = SDL_CreateWindow("Chess Board",
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
        }
    }

    play_game(renderer, board, textures);

    // Curăță resursele
    for (int i = 0; i < 12; i++) {
        SDL_DestroyTexture(textures[i]);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();
    free_chess_board(board);
    return 0;
}
