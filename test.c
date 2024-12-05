#include "chess.h"
#include <stdio.h>

int main() {
    int **board = create_chess_board();

    printf("Tabla de șah a fost creată!\n");

    free_chess_board(board);
    return 0;
}