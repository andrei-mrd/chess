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

void display_board(SDL_Renderer *renderer, SDL_Texture *textures[]) {
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
                if(i == 1) {
                    draw_piece(renderer, textures[1], i, j);
                }else if(i == 6) {
                    draw_piece(renderer, textures[0], i, j);
                }else if(i == 7) {
                    if(j == 0 || j == 7) {
                        draw_piece(renderer, textures[2], i, j);
                    }else if(j == 1 || j == 6) {
                        draw_piece(renderer, textures[6], i, j);
                    }else if(j == 2 || j == 5) {
                        draw_piece(renderer, textures[4], i, j);
                    }else if(j == 3) {
                        draw_piece(renderer, textures[8], i, j);
                    }else if(j == 4) {
                        draw_piece(renderer, textures[10], i, j);
                    }
                }else if(i == 0) {
                    if(j == 0 || j == 7) {
                        draw_piece(renderer, textures[3], i, j);
                    }else if(j == 1 || j == 6) {
                        draw_piece(renderer, textures[7], i, j);
                    }else if(j == 2 || j == 5) {
                        draw_piece(renderer, textures[5], i, j);
                    }else if(j == 3) {
                        draw_piece(renderer, textures[9], i, j);
                    }else if(j == 4) {
                        draw_piece(renderer, textures[11], i, j);
                    }
                }
            }
        }
        SDL_RenderPresent(renderer);
        return;
    }
}

