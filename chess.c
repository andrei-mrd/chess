#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "chess.h"

bool selectare_valida(int** table, int selected_x, int selected_y, char culoare){
    if(table[selected_y][selected_x] == 7 || table[selected_y][selected_x] == 8 || 
        (table[selected_y][selected_x] >= 1 && table[selected_y][selected_x] <= 6 && culoare == 'A') || 
        (table[selected_y][selected_x] >= -6 && table[selected_y][selected_x] <= -1 && culoare == 'N')) {
            return false;
    }
    return true;
}

char identifica_piesa(int** table, int x, int y) {
    switch (table[y][x])
    {
    case -1:
        //printf("pion alb\n");
        return('P');
        break;
    
    case 1:
        //printf("pion negru\n");
        return('P');
        break;
    
    case -2:
        //printf("nebun alb\n");
        return('N');
        break;
    
    case 2:
        //printf("nebun negru\n");
        return('N');
        break;
    
    case -3:
        //printf("cal alb\n");
        return('C');
        break;
    
    case 3:
        //printf("cal negru\n");
        return('C');
        break;
    
    case -4:
        //printf("tura alba\n");
        return('T');
        break;
    
    case 4:
        //printf("tura neagra\n");
        return('T');
        break;
    
    case -5:
        //printf("regina alba\n");
        return('Q');
        break;
    
    case 5:
        //printf("regina neagra\n");
        return('Q');
        break;
    
    case -6:
        //printf("rege alb\n");
        return('K');
        break;
    
    case 6:
        //printf("rege negru\n");
        return('K');
        break;

    default:
        break;
    }
}

int** create_chess_board() {
    int** board = (int**)malloc(8 * sizeof(int*));
    if (board == NULL) {
        perror("Eroare la alocarea memoriei pentru tabla");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 8; i++) {
        board[i] = (int*)malloc(8 * sizeof(int));
        if (board[i] == NULL) {
            perror("Eroare la alocarea memoriei pentru o linie din tabla");
            exit(EXIT_FAILURE);
        }
    }

    int defaultTable[8][8] = {
        { 4,  3,  2,  5,  6,  2,  3,  4}, // Linie 0: Piese majore negru
        { 1,  1,  1,  1,  1,  1,  1,  1}, // Linie 1: Pioni negru
        { 7,  8,  7,  8,  7,  8,  7,  8}, // Linii goale
        { 8,  7,  8,  7,  8,  7,  8,  7},
        { 7,  8,  7,  8,  7,  8,  7,  8},
        { 8,  7,  8,  7,  8,  7,  8,  7},
        {-1, -1, -1, -1, -1, -1, -1, -1}, // Linie 6: Pioni alb
        {-4, -3,  -2, -5, -6, -2, -3, -4}  // Linie 7: Piese majore alb
    };

    for (int i = 0; i < 8; i++) {
        memcpy(board[i], defaultTable[i], 8 * sizeof(int));
    }

    return board;
}

void update_board(int** table, int from_X, int from_Y, int to_X, int to_Y) {
    if(from_X % 2 == from_Y % 2) {
        //este loc alb de unde pleaca
        int aux = 7;
        table[to_Y][to_X] = table[from_Y][from_X];
        table[from_Y][from_X] = aux;
    }else {
        int aux = 8;
        table[to_Y][to_X] = table[from_Y][from_X];
        table[from_Y][from_X] = aux;
    }
}

void free_chess_board(int** board) {
    for (int i = 0; i < 8; i++) {
        free(board[i]);
    }
    free(board);
}

bool validare_miscare_pion(int** table, int current_X, int current_Y, int new_X, int new_Y, char culoare, 
                            int from_X, int from_Y, int to_X, int to_Y) {
    //from_X, from_Y, to_X, to_Y retin daca s-a miscat pionul advers pt cazul de en-passant
    //verificam daca pionul este alb
    if(culoare == 'A') {
        //verificam daca pe pozitia curenta se afla un pion alb
        if(table[current_Y][current_X] == -1) {
            //cazul I verificam daca suntem pe pozitia de start
            if(current_Y == 6) {
                //subcaz 1 -> o singura patratica inainte
                //trebuie sa verificam daca este libera casuta din fata
                if(current_X == new_X && new_Y == current_Y - 1 && (table[new_Y][current_X] == 7 || table[new_Y][current_X] == 8)) {
                    return true;
                }
                //subcaz 2 -> 2 patratele inainte
                //trebuie sa verificam ambele casute din fata sa nu fie ocupate
                if(current_X == new_X && new_Y == current_Y - 2 && (table[new_Y][current_X] == 7 || table[new_Y][current_X] == 8)
                    && (table[current_Y - 1][current_X] == 7 || table[current_Y - 1][current_X] == 8)) {
                    return true;
                }
                //subcaz 3 -> mancare piesa din diagonala(dreapta si stanga)
                //dreapta
                if(new_X == current_X + 1 && new_Y == current_Y - 1 && table[new_Y][new_X] != 7 && table[new_Y][new_X] != 8 && table[new_Y][new_X] != 6
                    && table[new_Y][new_X] != -1 && table[new_Y][new_X] != -2 && table[new_Y][new_X] != -3 && table[new_Y][new_X] != -4 && table[new_Y][new_X] != -5
                    && table[new_Y][new_X] != -6)  {
                    return true;
                }
                //stanga
                if(new_X == current_X - 1 && new_Y == current_Y - 1 && table[new_Y][new_X] != 7 && table[new_Y][new_X] != 8 && table[new_Y][new_X] != 6
                    && table[new_Y][new_X] != -1 && table[new_Y][new_X] != -2 && table[new_Y][new_X] != -3 && table[new_Y][new_X] != -4 && table[new_Y][new_X] != -5
                    && table[new_Y][new_X] != -6) {
                    return true;
                }
            }else {//cazul II pionul nu este pe pozitia initiala
                //subcazul 1 -> o singura patratica inainte
                if(new_X == current_X && new_Y == current_Y - 1 && (table[new_Y][new_X] == 7 || table[new_Y][new_X] == 8)) {
                    return true;
                }
                //subcazul 2 -> mancare piesa pe diagonala
                //dreapta
                if(new_X == current_X + 1 && new_Y == current_Y - 1 && table[new_Y][new_X] != 7 && table[new_Y][new_X] != 8 && table[new_Y][new_X] != 6
                    && table[new_Y][new_X] != -1 && table[new_Y][new_X] != -2 && table[new_Y][new_X] != -3 && table[new_Y][new_X] != -4 && table[new_Y][new_X] != -5
                    && table[new_Y][new_X] != -6) {
                    return true;
                }
                //stanga
                if(new_X == current_X - 1 && new_Y == current_Y - 1 && table[new_Y][new_X] != 7 && table[new_Y][new_X] != 8 && table[new_Y][new_X] != 6
                    && table[new_Y][new_X] != -1 && table[new_Y][new_X] != -2 && table[new_Y][new_X] != -3 && table[new_Y][new_X] != -4 && table[new_Y][new_X] != -5
                    && table[new_Y][new_X] != -6) {
                    return true;
                }
                //subcazul 3 -> en-passant
                if(from_X != -1 && from_Y != -1 && to_X != -1 && to_Y != -1) {
                    // conditii pentru en-passant
                    if(from_X == to_X && abs(to_Y - from_Y) == 2 && current_Y == to_Y && 
                       new_X == to_X && new_Y == current_Y - 1 && table[to_Y][to_X] == 1) {
                        return true;
                    }
                }
            }
        }
    }else if(culoare == 'N') {
        //verificam daca pe pozitia curenta se afla un pion negru
        if(table[current_Y][current_X] == 1) {
            //cazul I verificam daca suntem pe pozitia de start
            if(current_Y == 1) {
                //subcaz 1 -> o singura patratica inainte
                //trebuie sa verificam daca este libera casuta din fata
                if(current_X == new_X && new_Y == current_Y + 1 && (table[new_Y][current_X] == 7 || table[new_Y][current_X] == 8)) {
                    return true;
                }
                //subcaz 2 -> 2 patratele inainte
                //trebuie sa verificam ambele casute din fata sa nu fie ocupate
                if(current_X == new_X && new_Y == current_Y + 2 && (table[new_Y][current_X] == 7 || table[new_Y][current_X] == 8)
                    && (table[current_Y + 1][current_X] == 7 || table[current_Y + 1][current_X] == 8)) {
                    return true;
                }
                //subcaz 3 -> mancare piesa din diagonala(dreapta si stanga)
                //stanga
                if(new_X == current_X + 1 && new_Y == current_Y + 1 && table[new_Y][new_X] != 7 && table[new_Y][new_X] != 8 && table[new_Y][new_X] != -6
                    && table[new_Y][new_X] != 1 && table[new_Y][new_X] != 2 && table[new_Y][new_X] != 3 && table[new_Y][new_X] != 4 && table[new_Y][new_X] != 5
                    && table[new_Y][new_X] != 6 ){
                    return true;
                }
                //dreapta
                if(new_X == current_X - 1 && new_Y == current_Y + 1 && table[new_Y][new_X] != 7 && table[new_Y][new_X] != 8 && table[new_Y][new_X] != -6
                    && table[new_Y][new_X] != 1 && table[new_Y][new_X] != 2 && table[new_Y][new_X] != 3 && table[new_Y][new_X] != 4 && table[new_Y][new_X] != 5
                    && table[new_Y][new_X] != 6) {
                    return true;
                }
            }else {//cazul II pionul nu este pe pozitia initiala
                //subcazul 1 -> o singura patratica inainte
                if(new_X == current_X && new_Y == current_Y + 1 && (table[new_Y][new_X] == 7 || table[new_Y][new_X] == 8)) {
                    return true;
                }
                //subcazul 2 -> mancare piesa pe diagonala
                //stanga
                if(new_X == current_X + 1 && new_Y == current_Y + 1 && table[new_Y][new_X] != 7 && table[new_Y][new_X] != 8 && table[new_Y][new_X] != -6
                    && table[new_Y][new_X] != 1 && table[new_Y][new_X] != 2 && table[new_Y][new_X] != 3 && table[new_Y][new_X] != 4 && table[new_Y][new_X] != 5
                    && table[new_Y][new_X] != 6 ) {
                    return true;
                }
                //dreapta
                if(new_X == current_X - 1 && new_Y == current_Y + 1 && table[new_Y][new_X] != 7 && table[new_Y][new_X] != 8 && table[new_Y][new_X] != -6
                    && table[new_Y][new_X] != 1 && table[new_Y][new_X] != 2 && table[new_Y][new_X] != 3 && table[new_Y][new_X] != 4 && table[new_Y][new_X] != 5
                    && table[new_Y][new_X] != 6 ) {
                    return true;
                }
                //subcazul 3 -> en-passant
                if(from_X != -1 && from_Y != -1 && to_X != -1 && to_Y != -1) {
                    // conditii pentru en-passant
                    //board, 6, 4, 5, 5, 'N', 5, 7, 5, 5
                    if(from_X == to_X && abs(to_Y - from_Y) == 2 && current_Y == to_Y && 
                       new_X == to_X && new_Y == current_Y + 1 && table[to_Y][to_X] == -1) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool validare_miscare_tura(int** table, int current_X, int current_Y, int new_X, int new_Y, char culoare) {
    //Verificam daca piesa este o tura de culoarea corecta
    if ((culoare == 'A' && table[current_Y][current_X] != -4) || (culoare == 'N' && table[current_Y][current_X] != 4)) {
        return false;
    }
    
    //Verificam daca mutarea este pe linie dreapta
    if (current_X == new_X) {  //miscare pe verticala
        int step = (new_Y > current_Y) ? 1 : -1;
        for (int i = current_Y + step; i != new_Y; i += step) {
            if (table[i][current_X] != 7 && table[i][current_X] != 8) { //calea nu e libera
                return false;
            }
        }
    } else if (current_Y == new_Y) {  //miscare pe orizontala
        int step = (new_X > current_X) ? 1 : -1;
        for (int i = current_X + step; i != new_X; i += step) {
            if (table[current_Y][i] != 7 && table[current_Y][i] != 8) { //calea nu e libera
                return false;
            }
        }
    } else {
        return false;  //mutare invalida pentru tura
    }

    //Verificm daca destinatia este ocupata de o piesă de aceeasi culoare
    if ((culoare == 'A' && table[new_Y][new_X] < 0 && table[new_Y][new_X] > -6) ||
        (culoare == 'N' && table[new_Y][new_X] > 0 && table[new_Y][new_X] < 7)) {
        return false;
    }

    return true;  //mutare valida
}


bool validare_miscare_nebun(int** table, int current_X, int current_Y, int new_X, int new_Y, char culoare) {
    //verificam daca piesa actuala e nebun + culoarea care trebuie
    if ((culoare == 'A' && table[current_Y][current_X] != -2) || 
        (culoare == 'N' && table[current_Y][current_X] != 2)) {
        return false;
    }

    //verificam daca sunt valide coordonatele pentru o miscare de nebun
    if(abs(new_X - current_X) != abs(new_Y-current_Y)) {
        return false;
    }

    //verificam daca e destinatia potrivita pt fiecare culoare
    if(culoare == 'A') {
        if(table[new_Y][new_X] == -1 || table[new_Y][new_X] == -2 || table[new_Y][new_X] == -3 || table[new_Y][new_X] == -4 || table[new_Y][new_X] == -5
            || table[new_Y][new_X] == -6 || table[new_Y][new_X] == 6) {
                return false;
        }
    }else if(culoare == 'N') {  
        if(table[new_Y][new_X] == 1 || table[new_Y][new_X] == 2 || table[new_Y][new_X] == 3 || table[new_Y][new_X] == 4 || table[new_Y][new_X] == 5
            || table[new_Y][new_X] == -6 || table[new_Y][new_X] == 6) {
                return false;
        }
    }

    if(new_X < current_X && new_Y < current_Y) {
        //directia e stanga sus
        for(int i = current_X - 1, j = current_Y - 1; i > new_X && j > new_Y; i--, j--) {
            if(table[j][i] != 7 && table[j][i] != 8) {
                return false;
            }
        }
        return true;
    }else if(new_X < current_X && new_Y > current_Y) {
        //directia e stanga jos
        for(int i = current_X - 1, j = current_Y + 1; i > new_X && j < new_Y; i--, j++) {
            if(table[j][i] != 7 && table[j][i] != 8) {
                return false;
            }
        }
        return true;
    }else if(new_X > current_X && new_Y < current_Y) {
        //directia e dreapta sus
        for(int i = current_X + 1, j = current_Y - 1; i < new_X && j > new_Y; i++, j--) {
            if(table[j][i] != 7 && table[j][i] != 8) {
                return false;
            }
        }
        return true;
    }else if(new_X > current_X && new_Y > current_Y) {
        //directia e dreapta jos
        for(int i = current_X + 1, j = current_Y + 1; i < new_X && j < new_Y; i++, j++) {
            if(table[j][i] != 7 && table[j][i] != 8) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool validare_miscare_cal(int** table, int current_X, int current_Y, int new_X, int new_Y, char culoare) {
    //verificam daca la pozitia actuala se afla un cal
    if ((culoare == 'A' && table[current_Y][current_X] != -3) || 
        (culoare == 'N' && table[current_Y][current_X] != 3)) {
        return false;
    }
    //calul are 8 variante de miscare
    
    //acoperim 4 cazuri, sus stanga/dreapta, jos stanga/dreapta
    if(abs(new_X - current_X) == 1 && abs(new_Y - current_Y) == 2) {
        if(culoare == 'A') {
            if(table[new_Y][new_X] == 7 || table[new_Y][new_X] == 8 || (table[new_Y][new_X] < 6 && table[new_Y][new_X] > 0)) {
                return true;
            }
        }else if(culoare == 'N') {
            if(table[new_Y][new_X] == 7 || table[new_Y][new_X] == 8 || (table[new_Y][new_X] > -6 && table[new_Y][new_X] < 0)) {
                return true;
            }
        }
    }else if(abs(new_X - current_X) == 2 && abs(new_Y - current_Y) == 1) {
        //acoperim celelalte 4 cazuri, stanga sus/jos, dreapta sus/jos
        if(culoare == 'A') {
            if(table[new_Y][new_X] == 7 || table[new_Y][new_X] == 8 || (table[new_Y][new_X] < 6 && table[new_Y][new_X] > 0)) {
                return true;
            }
        }else if(culoare == 'N') {
            if(table[new_Y][new_X] == 7 || table[new_Y][new_X] == 8 || (table[new_Y][new_X] > -6 && table[new_Y][new_X] < 0)) {
                return true;
            }
        }
    }
    return false;
}

bool validare_miscare_regina(int** table, int current_X, int current_Y, int new_X, int new_Y, char culoare) {
    //verificam daca la pozitia actuala se afla o regina
    if ((culoare == 'A' && table[current_Y][current_X] != -5) || 
        (culoare == 'N' && table[current_Y][current_X] != 5)) {
        return false;
    }

    //verificam daca sunt valide coordonatele pentru o miscare de nebun
    if(abs(new_X - current_X) == abs(new_Y-current_Y)) { 
        //verificam daca e destinatia potrivita pt fiecare culoare
        if(culoare == 'A') {
            if(table[new_Y][new_X] == -1 || table[new_Y][new_X] == -2 || table[new_Y][new_X] == -3 || table[new_Y][new_X] == -4 || table[new_Y][new_X] == -5
                || table[new_Y][new_X] == -6 || table[new_Y][new_X] == 6) {
                    printf("A crapat aici2\n");
                    return false;
            }
        }else if(culoare == 'N') {  
            if(table[new_Y][new_X] == 1 || table[new_Y][new_X] == 2 || table[new_Y][new_X] == 3 || table[new_Y][new_X] == 4 || table[new_Y][new_X] == 5
                || table[new_Y][new_X] == -6 || table[new_Y][new_X] == 6) {
                    printf("A crapat aici3\n");
                    return false;
            }
        }

        if(new_X < current_X && new_Y < current_Y) {
            //directia e stanga sus
            for(int i = current_X - 1, j = current_Y - 1; i > new_X && j > new_Y; i--, j--) {
                if(table[j][i] != 7 && table[j][i] != 8) {
                    printf("A crapat aici4\n");
                    return false;
                }
            }
            return true;
        }else if(new_X < current_X && new_Y > current_Y) {
            //directia e stanga jos
            for(int i = current_X - 1, j = current_Y + 1; i > new_X && j < new_Y; i--, j++) {
                if(table[j][i] != 7 && table[j][i] != 8) {printf("A crapat aici5\n");
                    return false;
                }
            }
            return true;
        }else if(new_X > current_X && new_Y < current_Y) {
            //directia e dreapta sus
            for(int i = current_X + 1, j = current_Y - 1; i < new_X && j > new_Y; i++, j--) {
                if(table[j][i] != 7 && table[j][i] != 8) {
                    printf("A crapat aici6\n");
                    return false;
                }
            }
            return true;
        }else if(new_X > current_X && new_Y > current_Y) {
            //directia e dreapta jos
            for(int i = current_X + 1, j = current_Y + 1; i < new_X && j < new_Y; i++, j++) {
                if(table[j][i] != 7 && table[j][i] != 8) {
                    printf("A crapat aici7\n");
                    return false;
                }
            }
            return true;
        }
    }

    //Verificam daca mutarea este pe linie dreapta
    if (current_X == new_X) {  //miscare pe verticala
        int step = (new_Y > current_Y) ? 1 : -1;
        for (int i = current_Y + step; i != new_Y; i += step) {
            if (table[i][current_X] != 7 && table[i][current_X] != 8) { //calea nu e libera
            printf("A crapat aici8\n");
                return false;
            }
        }
    } else if (current_Y == new_Y) {  //miscare pe orizontala
        int step = (new_X > current_X) ? 1 : -1;
        for (int i = current_X + step; i != new_X; i += step) {
            if (table[current_Y][i] != 7 && table[current_Y][i] != 8) { //calea nu e libera
                printf("A crapat aici9\n");
                return false;
            }
        }
    } else {
        printf("A crapat aici10\n");
        return false;  //mutare invalida pentru tura
    }

    //Verificm daca destinatia este ocupata de o piesă de aceeasi culoare
    if ((culoare == 'A' && table[new_Y][new_X] < 0 && table[new_Y][new_X] > -6) ||
        (culoare == 'N' && table[new_Y][new_X] > 0 && table[new_Y][new_X] < 7)) {
        printf("A crapat aici11\n");
        return false;
    }

    return true;
}

bool validare_miscare_rege(int** table, int current_X, int current_Y, int new_X, int new_Y, char culoare) {
    //verificam daca la pozitia actuala se afla un cal
    if ((culoare == 'A' && table[current_Y][current_X] != -6) || 
        (culoare == 'N' && table[current_Y][current_X] != 6)) {
        return false;
    }

    //regele are 8 miscari valabile, 4 pe diagonala, 2 verticale, 2 orizontale

    //cele 4 de pe diagonala
    if((abs(new_X - current_X) == 1 && abs(new_Y - current_Y) == 1) ) {
        if(culoare == 'A') {
            if((table[new_Y][new_X] < 6 && table[new_Y][new_X] > 0) || (table[new_Y][new_X] == 7 || table[new_Y][new_X] == 8)) {
                return true;
            }
        }else if(culoare == 'N') {
            if((table[new_Y][new_X] > -6 && table[new_Y][new_X] < 0) || (table[new_Y][new_X] == 7 || table[new_Y][new_X] == 8)) {
                return true;
            }
        }
    }

    //cele 2 de pe verticala
    if((abs(new_X - current_X) == 0 && abs(new_Y - current_Y) == 1)) {
        if(culoare == 'A') {
            if((table[new_Y][new_X] < 6 && table[new_Y][new_X] > 0) || (table[new_Y][new_X] == 7 || table[new_Y][new_X] == 8)) {
                return true;
            }
        }else if(culoare == 'N') {
            if((table[new_Y][new_X] > -6 && table[new_Y][new_X] < 0) || (table[new_Y][new_X] == 7 || table[new_Y][new_X] == 8)) {
                return true;
            }
        }
    }

    //cele 2 de pe orizontala
    if((abs(new_X - current_X) == 1 && abs(new_Y - current_Y) == 0) ) {
        if(culoare == 'A') {
            if((table[new_Y][new_X] < 6 && table[new_Y][new_X] > 0) || (table[new_Y][new_X] == 7 || table[new_Y][new_X] == 8)) {
                return true;
            }
        }else if(culoare == 'N') {
            if((table[new_Y][new_X] > -6 && table[new_Y][new_X] < 0) || (table[new_Y][new_X] == 7 || table[new_Y][new_X] == 8)) {
                return true;
            }
        }
    }

    return false;
}


bool este_in_sah(int** table, char culoare) {
    //cordonate_x, coordonate_y sunt cele ale regelui
    int coordonate_x, coordonate_y;
    if(culoare == 'A') {
        coordonate_x = 4; 
        coordonate_y = 7;
        for(int i = 0; i<8 ;i++) {
            for(int j = 0; j< 8; j++) {
                if(table[i][j] == -6) {
                    coordonate_y = i;
                    coordonate_x = j;
                    break;
                }
            }
        }
        for(int i = 0; i<8; i++) {
            for(int j = 0; j<8; j++) {
                //verificam daca gasim o piesa de culoare opusa
                if(table[i][j] >= 1 && table[i][j] <= 6) {
                    //verificam pt fiecare piesa in parte daca aceasta poate da sah regelui
                    if(validare_miscare_pion(table, j, i, coordonate_x, coordonate_y, 'N', -1, -1, -1, -1) == true) {
                        return true;
                    }

                    if(validare_miscare_tura(table, j, i, coordonate_x, coordonate_y, 'N') == true) {
                        return true;
                    }

                    if(validare_miscare_cal(table, j, i, coordonate_x, coordonate_y, 'N') == true) {
                        return true;
                    }

                    if(validare_miscare_nebun(table, j, i, coordonate_x, coordonate_y, 'N') == true) {
                        return true;
                    }

                    if(validare_miscare_regina(table, j, i, coordonate_x, coordonate_y, 'N') == true) {
                        return true;
                    }

                    if(validare_miscare_rege(table, j, i, coordonate_x, coordonate_y, 'N') == true) {
                        return true;
                    }
                }
            }
        }
    }else if(culoare == 'N') {
        coordonate_x = 4; 
        coordonate_y = 0;
        for(int i = 0; i<8 ;i++) {
            for(int j = 0; j< 8; j++) {
                if(table[i][j] == 6) {
                    coordonate_y = i;
                    coordonate_x = j;
                    break;
                }
            }
        }
        for(int i = 0; i<8; i++) {
            for(int j = 0; j<8; j++) {
                //verificam daca gasim o piesa de culoare opusa
                if(table[i][j] >= -6 && table[i][j] <= -1) {
                    //verificam pt fiecare piesa in parte daca aceasta poate da sah regelui
                    if(validare_miscare_pion(table, j, i, coordonate_x, coordonate_y, 'A', -1, -1, -1, -1) == true) {
                        return true;
                    }

                    if(validare_miscare_tura(table, j, i, coordonate_x, coordonate_y, 'A') == true) {
                        return true;
                    }

                    if(validare_miscare_cal(table, j, i, coordonate_x, coordonate_y, 'A') == true) {
                        return true;
                    }

                    if(validare_miscare_nebun(table, j, i, coordonate_x, coordonate_y, 'A') == true) {
                        return true;
                    }

                    if(validare_miscare_regina(table, j, i, coordonate_x, coordonate_y, 'A') == true) {
                        return true;
                    }

                    if(validare_miscare_rege(table, j, i, coordonate_x, coordonate_y, 'A') == true) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

/*
* P - pion
* T - tura
* C - cal
* N - nebun
* Q - regina(queen)
* K - king(rege)
* functia returneaza true daca regele e in sah, sau urmeaza sa intre dupa mutarea specificata in argumentele functiei
*/

bool intra_in_sah(int** table, char piesa, char culoare, int from_X, int from_Y, int to_X, int to_Y, 
                            bool is_enPassant, int enp_old_X, int enp_old_Y, int enp_new_X, int enp_new_Y) {
    int copie_tabel[8][8];
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            copie_tabel[i][j] = table[i][j];
        }
    }

    if(culoare == 'A') {
        int coordonate_x = 4, coordonate_y = 7; //pt rege
        for(int i = 0; i<8 ;i++) {
            for(int j = 0; j< 8; j++) {
                if(copie_tabel[i][j] == -6) {
                    coordonate_y = i;
                    coordonate_x = j;
                    break;
                }
            }
        }

        //verificam daca dupa mutarea specificata in argumente, regele va intra in sah
        if(piesa == 'P') {
            if(is_enPassant == true) {
                if(validare_miscare_pion((int**) copie_tabel, from_X, from_Y, to_X, to_Y, 'A', enp_old_X, enp_old_Y, enp_new_X, enp_new_Y) == true) {
                    //interschimbare, deoarece e pe diagonala
                    int aux = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = copie_tabel[to_Y][to_X];
                    copie_tabel[to_Y][to_X] = aux;
                }
            }else {
                if(validare_miscare_pion((int**) copie_tabel, from_X, from_Y, to_X, to_Y, 'A', -1, -1, -1, -1) == true) {
                    if(abs(from_X - to_X) == 1 && abs(from_Y - to_Y) == 1 || abs(from_Y - to_Y) == 2) {
                        int aux = copie_tabel[from_Y][from_X];
                        copie_tabel[from_Y][from_X] = copie_tabel[to_Y][to_X];
                        copie_tabel[to_Y][to_X] = aux;
                    }else if(abs(from_Y - to_Y) == 1) {
                        if(copie_tabel[to_Y][to_X] == 7) {
                            int aux = 8;
                            copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                            copie_tabel[from_Y][from_X] = aux;
                        }else if(copie_tabel[to_Y][to_X] == 8) {
                            int aux = 7;
                            copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                            copie_tabel[from_Y][from_X] = aux;
                        }
                    }
                }
            }
        }else if(piesa == 'T') {
            if(validare_miscare_tura((int**) copie_tabel, from_X, from_Y, to_X, to_Y, 'A') == true) {
                if(from_X % 2 == from_Y % 2) {
                    //culoare alba de unde pleaca
                    int aux = 7;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }else {
                    int aux = 8;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }
            }
        }else if(piesa == 'C') {
            if(validare_miscare_cal((int** ) copie_tabel, from_X, from_Y, to_X, to_Y, 'A') == true) {
                if(from_X % 2 == from_Y % 2) {
                    //culoare alba de unde pleaca
                    int aux = 7;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }else {
                    int aux = 8;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }
            }
        }else if(piesa == 'N') {
            if(validare_miscare_nebun((int** ) copie_tabel, from_X, from_Y, to_X, to_Y, 'A') == true) {
                if(from_X % 2 == from_Y % 2) {
                    //culoare alba de unde pleaca
                    int aux = 7;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }else {
                    int aux = 8;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }
            }
        }else if(piesa == 'Q') {
            if(validare_miscare_regina((int** ) copie_tabel, from_X, from_Y, to_X, to_Y, 'A') == true) {
                if(from_X % 2 == from_Y % 2) {
                    //culoare alba de unde pleaca
                    int aux = 7;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }else {
                    int aux = 8;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }
            }
        }else if(piesa == 'K') {
            if(validare_miscare_rege((int** ) copie_tabel, from_X, from_Y, to_X, to_Y, 'A') == true) {
                if(from_X % 2 == from_Y % 2) {
                    //culoare alba de unde pleaca
                    int aux = 7;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }else {
                    int aux = 8;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }
            }
        }

        if(este_in_sah((int** ) copie_tabel, 'A') == true) {
            return true;
        }else {
            return false;
        }
    }else if(culoare == 'N') {
        int coordonate_x = 4, coordonate_y = 0; //pt rege
        for(int i = 0; i<8 ;i++) {
            for(int j = 0; j< 8; j++) {
                if(copie_tabel[i][j] == 6) {
                    coordonate_y = i;
                    coordonate_x = j;
                    break;
                }
            }
        }

        //verificam daca dupa mutarea specificata in argumente, regele va intra in sah
        if(piesa == 'P') {
            if(is_enPassant == true) {
                if(validare_miscare_pion((int** ) copie_tabel, from_X, from_Y, to_X, to_Y, 'N', enp_old_X, enp_old_Y, enp_new_X, enp_new_Y) == true) {
                    //interschimbare, deoarece e pe diagonala
                    int aux = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = copie_tabel[to_Y][to_X];
                    copie_tabel[to_Y][to_X] = aux;
                }
            }else {
                if(validare_miscare_pion((int** ) copie_tabel, from_X, from_Y, to_X, to_Y, 'N', -1, -1, -1, -1) == true) {
                    if(abs(from_X - to_X) == 1 && abs(from_Y - to_Y) == 1 || abs(from_Y - to_Y) == 2) {
                        int aux = copie_tabel[from_Y][from_X];
                        copie_tabel[from_Y][from_X] = copie_tabel[to_Y][to_X];
                        copie_tabel[to_Y][to_X] = aux;
                    }else if(abs(from_Y - to_Y) == 1) {
                        if(copie_tabel[to_Y][to_X] == 7) {
                            int aux = 8;
                            copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                            copie_tabel[from_Y][from_X] = aux;
                        }else if(copie_tabel[to_Y][to_X] == 8) {
                            int aux = 7;
                            copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                            copie_tabel[from_Y][from_X] = aux;
                        }
                    }
                }
            }
        }else if(piesa == 'T') {
            if(validare_miscare_tura((int** ) copie_tabel, from_X, from_Y, to_X, to_Y, 'N') == true) {
                if(from_X % 2 == from_Y % 2) {
                    //culoare alba de unde pleaca
                    int aux = 7;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }else {
                    int aux = 8;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }
            }
        }else if(piesa == 'C') {
            if(validare_miscare_cal((int** ) copie_tabel, from_X, from_Y, to_X, to_Y, 'N') == true) {
                if(from_X % 2 == from_Y % 2) {
                    //culoare alba de unde pleaca
                    int aux = 7;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }else {
                    int aux = 8;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }
            }
        }else if(piesa == 'N') {
            if(validare_miscare_nebun((int** ) copie_tabel, from_X, from_Y, to_X, to_Y, 'N') == true) {
                if(from_X % 2 == from_Y % 2) {
                    //culoare alba de unde pleaca
                    int aux = 7;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }else {
                    int aux = 8;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }
            }
        }else if(piesa == 'Q') {
            if(validare_miscare_regina((int** ) copie_tabel, from_X, from_Y, to_X, to_Y, 'N') == true) {
                if(from_X % 2 == from_Y % 2) {
                    //culoare alba de unde pleaca
                    int aux = 7;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }else {
                    int aux = 8;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }
            }
        }else if(piesa == 'K') {
            if(validare_miscare_rege((int** ) copie_tabel, from_X, from_Y, to_X, to_Y, 'N') == true) {
                if(from_X % 2 == from_Y % 2) {
                    //culoare alba de unde pleaca
                    int aux = 7;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }else {
                    int aux = 8;
                    copie_tabel[to_Y][to_X] = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = aux;
                }
            }
        }

        if(este_in_sah((int** ) copie_tabel, 'N') == true) {
            return true;
        }else {
            return false;
        }
    }
}