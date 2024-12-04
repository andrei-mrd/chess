#ifndef CHESS_H
#define CHESS_H

int** createChessBoard();
void update_board(int** table, int from_X, int from_Y, int to_X, int to_Y);
void free_chess_board(int** board);
void display_board();
bool validare_miscare_pion(int** table, int current_X, int current_Y, int new_X, int new_Y, char culoare, int from_X, int from_Y, int to_X, int to_Y);
bool validare_miscare_nebun(int** table, int current_X, int current_Y, int new_X, int new_Y, char culoare);
bool validare_miscare_cal(int** table, int current_X, int current_Y, int new_X, int new_Y, char culoare);
bool validare_miscare_regina(int** table, int current_X, int current_Y, int new_X, int new_Y, char culoare);
bool validare_micare_rege(int** table, int current_X, int current_Y, int new_X, int new_Y, char culoare);
bool este_in_sah(int** table, char culoare);
bool intra_in_sah(int** table, char piesa, char culoare, int from_X, int from_Y, int to_X, int to_Y, bool is_enPassant, int enp_old_X, int enp_old_Y, int enp_new_X, int enp_new_Y);

#endif
