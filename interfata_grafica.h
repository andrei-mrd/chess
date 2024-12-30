#include "chess.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#ifndef INTERFATA_GRAFICA_H
#define INTERFATA_GRAFICA_H

void draw_board(SDL_Renderer *renderer);
void draw_piece(SDL_Renderer *renderer, SDL_Texture *piece, int row, int col);
void display_board(SDL_Renderer *renderer ,SDL_Texture *textures[]);

#endif