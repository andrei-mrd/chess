#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

void display_board(int** table, char culoare) {
    printf("    A  B  C  D  E  F  G  H\n");

    for (int i = 0; i < 8; i++) {
        int row = culoare == 'A' ? i : 7 - i;  //inversam pentru negru tabla
        printf(" %d ", 8 - row); // Numerotare rânduri (1-8 pentru negru, 8-1 pentru alb)

        for (int j = 0; j < 8; j++) {
            switch (table[row][j]) {
                case 1:  printf(" ♙ "); break;  // Pion negru
                case 2:  printf(" ♗ "); break;  // Nebun negru
                case 3:  printf(" ♘ "); break;  // Cal negru
                case 4:  printf(" ♖ "); break;  // Tura negru
                case 5:  printf(" ♕ "); break;  // Regina negru
                case 6:  printf(" ♔ "); break;  // Regele negru
                case -1: printf(" ♟ "); break;  // Pion alb
                case -3: printf(" ♞ "); break;  // Cal alb
                case -2: printf(" ♝ "); break;  // Nebun alb
                case -4: printf(" ♜ "); break;  // Tura alba
                case -5: printf(" ♛ "); break;  // Regina alba
                case -6: printf(" ♚ "); break;  // Regele negru
                case 7:  printf("   "); break;  // Casuta libera
                case 8:  printf(" # "); break;  // Casuta blocata
                default: break;
            }
        }
        printf(" %d\n", 8 - row); // Numerotare rânduri (1-8 pentru negru, 8-1 pentru alb)
    }

    // Afișează capul de tabel cu coloanele A-H
    printf("    A  B  C  D  E  F  G  H\n");
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

bool validare_micare_rege(int** table, int current_X, int current_Y, int new_X, int new_Y, char culoare) {
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
                if(validare_miscare_pion(copie_tabel, from_X, from_Y, to_X, to_Y, 'A', enp_old_X, enp_old_Y, enp_new_X, enp_new_Y) == true) {
                    //interschimbare, deoarece e pe diagonala
                    int aux = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = copie_tabel[to_Y][to_X];
                    copie_tabel[to_Y][to_X] = aux;
                }
            }else {
                if(validare_miscare_pion(copie_tabel, from_X, from_Y, to_X, to_Y, 'A', -1, -1, -1, -1) == true) {
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
            if(validare_miscare_tura(copie_tabel, from_X, from_Y, to_X, to_Y, 'A') == true) {
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
            if(validare_miscare_cal(copie_tabel, from_X, from_Y, to_X, to_Y, 'A') == true) {
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
            if(validare_miscare_nebun(copie_tabel, from_X, from_Y, to_X, to_Y, 'A') == true) {
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
            if(validare_miscare_regina(copie_tabel, from_X, from_Y, to_X, to_Y, 'A') == true) {
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
            if(validare_miscare_rege(copie_tabel, from_X, from_Y, to_X, to_Y, 'A') == true) {
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

        if(este_in_sah(copie_tabel, 'A') == true) {
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
                if(validare_miscare_pion(copie_tabel, from_X, from_Y, to_X, to_Y, 'N', enp_old_X, enp_old_Y, enp_new_X, enp_new_Y) == true) {
                    //interschimbare, deoarece e pe diagonala
                    int aux = copie_tabel[from_Y][from_X];
                    copie_tabel[from_Y][from_X] = copie_tabel[to_Y][to_X];
                    copie_tabel[to_Y][to_X] = aux;
                }
            }else {
                if(validare_miscare_pion(copie_tabel, from_X, from_Y, to_X, to_Y, 'N', -1, -1, -1, -1) == true) {
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
            if(validare_miscare_tura(copie_tabel, from_X, from_Y, to_X, to_Y, 'N') == true) {
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
            if(validare_miscare_cal(copie_tabel, from_X, from_Y, to_X, to_Y, 'N') == true) {
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
            if(validare_miscare_nebun(copie_tabel, from_X, from_Y, to_X, to_Y, 'N') == true) {
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
            if(validare_miscare_regina(copie_tabel, from_X, from_Y, to_X, to_Y, 'N') == true) {
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
            if(validare_miscare_rege(copie_tabel, from_X, from_Y, to_X, to_Y, 'N') == true) {
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

        if(este_in_sah(copie_tabel, 'N') == true) {
            return true;
        }else {
            return false;
        }
    }
}


int main() {
    int** board = create_chess_board();
    display_board(board, 'A');
    //printf("\n");
    
    /*
    1.Pion pozitie initiala
      alb
    printf("1. Pion in pozitie initiala\n");
    printf("alb\n");
    if(validare_miscare_pion(board, 0, 6, 0, 5, 'A', -1, -1, -1, -1) == true) {
        printf("o patratica in fata functioneaza\n");
    }else {
        printf("nu functioneaza o patratica in fata functioneaza\n");
    }

    if(validare_miscare_pion(board, 0, 6, 0, 4, 'A', -1, -1, -1, -1) == true) {
        printf("2 patratele in fata functioneaza\n");
    }else {
        printf("nu functioneaza 2 patratele in fata functioneaza\n");
    }
    
    if(validare_miscare_pion(board, 5, 6, 6, 5, 'A', -1, -1, -1, -1) == true) {
        printf("luatul pe diagonala dreapta functioneaza\n");
    }else {
        printf("nu functioneaza luarea in diagonala dreapta\n");
    }

    if(validare_miscare_pion(board, 7, 6, 6, 5, 'A', -1, -1, -1, -1) == true) {
        printf("luatul pe diagonala stanga functioneaza\n");
    }else {
        printf("nu functioneaza luarea in stanga dreapta\n");
    }

    //negru
    printf("negru\n");
    if(validare_miscare_pion(board, 7, 1, 7, 2, 'N', -1, -1, -1, -1) == true) {
        printf("o patratica in fata functioneaza\n");
    }else {
        printf("nu functioneaza o patratica in fata functioneaza\n");
    }

    if(validare_miscare_pion(board, 7, 1, 7, 3, 'N', -1, -1, -1, -1) == true) {
        printf("2 patratele in fata functioneaza\n");
    }else {
        printf("nu functioneaza 2 patratele in fata functioneaza\n");
    }
    
    if(validare_miscare_pion(board, 2, 1, 3, 2, 'N', -1, -1, -1, -1) == true) {
        printf("luatul pe diagonala dreapta functioneaza\n");
    }else {
        printf("nu functioneaza luarea in diagonala dreapta\n");
    }

    if(validare_miscare_pion(board, 2, 1, 1, 2, 'N', -1, -1, -1, -1) == true) {
        printf("luatul pe diagonala stanga functioneaza\n");
    }else {
        printf("nu functioneaza luarea in stanga dreapta\n");
    }

    //2. Pion in poz normala
    //alb
    printf("2. Pion in pozitie initiala\n");
    printf("Alb\n");
    if(validare_miscare_pion(board, 6, 3, 6, 2, 'A', -1, -1, -1, -1) == true) {
        printf("o patratica inainte functioneaza\n");
    }else {
        printf("o patratica inainte nu functioneaza\n");
    }

    if(validare_miscare_pion(board, 3, 3, 2, 2, 'A', -1, -1, -1, -1) == true) {
        printf("luarea de pion pe diagonala stanga functioneaza\n");
    }else {
        printf("luarea de pion pe diagonala stanga nu functioneaza\n");
    }

    if(validare_miscare_pion(board, 3, 3, 4, 2, 'A', -1, -1, -1, -1) == true) {
        printf("luarea de pion pe diagonala dreapta functioneaza\n");
    }else {
        printf("luarea de pion pe diagonala stanga nu functioneaza\n");
    }

    if(validare_miscare_pion(board, 6, 3, 5, 2, 'A', 5, 1, 5, 3) == true) {
        printf("en-passant stanga functioneaza\n");
    }else {
        printf("en-passant stanga nu functioneaza\n");
    }

    if(validare_miscare_pion(board, 6, 3, 7, 2, 'A', 7, 1, 7, 3) == true) {
        printf("en-passant dreapta functioneaza\n");
    }else {
        printf("en-passant dreapta nu functioneaza\n");
    }

    printf("Negru\n");
    if(validare_miscare_pion(board, 7, 1, 7, 2, 'N', -1, -1, -1, -1) == true) {
        printf("o patratica inainte functioneaza\n");
    }else {
        printf("o patratica inainte nu functioneaza\n");
    }

    if(validare_miscare_pion(board, 2, 2, 1, 3, 'N', -1, -1, -1, -1) == true) {
        printf("luarea de pion pe diagonala stanga functioneaza\n");
    }else {
        printf("luarea de pion pe diagonala stanga nu functioneaza\n");
    }

    if(validare_miscare_pion(board, 2, 2, 3, 3, 'N', -1, -1, -1, -1) == true) {
        printf("luarea de pion pe diagonala dreapta functioneaza\n");
    }else {
        printf("luarea de pion pe diagonala dreapta nu functioneaza\n");
    }

    if(validare_miscare_pion(board, 6, 4, 5, 5, 'N', 5, 6, 5, 4) == true) {
        printf("en-passant stanga functioneaza\n");
    }else {
        printf("en-passant stanga nu functioneaza\n");
    }

    if(validare_miscare_pion(board, 6, 4, 7, 5, 'N', 7, 6, 7, 4) == true) {
        printf("en-passant dreapta functioneaza\n");
    }else {
        printf("en-passant dreapta nu functioneaza\n");
    }
    3. Tura
    alb
    printf("Alb\n");
    if(validare_miscare_tura(board, 3, 4, 3, 0, 'A')) {
        printf("miscarea in sus functioneaza\n");
    }else {
        printf("miscarea in sus nu functioneaza\n");
    }

    if(validare_miscare_tura(board, 3, 4, 3, 7, 'A')) {
        printf("miscarea in jos functioneaza\n");
    }else {
        printf("miscarea in jos nu functioneaza\n");
    }

    if(validare_miscare_tura(board, 3, 4, 0, 4, 'A')) {
        printf("miscarea in stanga functioneaza\n");
    }else {
        printf("miscarea in stanga nu functioneaza\n");
    }

    if(validare_miscare_tura(board, 3, 4, 7, 4, 'A')) {
        printf("miscarea in dreapta functioneaza\n");
    }else {
        printf("miscarea in dreapta nu functioneaza\n");
    }

    //negru
    printf("negru\n");
    if(validare_miscare_tura(board, 4, 3, 4, 0, 'N')) {
        printf("miscarea in sus functioneaza\n");
    }else {
        printf("miscarea in sus nu functioneaza\n");
    }

    if(validare_miscare_tura(board, 4, 3, 4, 7, 'N')) {
        printf("miscarea in jos functioneaza\n");
    }else {
        printf("miscarea in jos nu functioneaza\n");
    }

    if(validare_miscare_tura(board, 4, 3, 0, 3, 'N')) {
        printf("miscarea in stanga functioneaza\n");
    }else {
        printf("miscarea in stanga nu functioneaza\n");
    }

    if(validare_miscare_tura(board, 4, 3, 7, 3, 'N')) {
        printf("miscarea in dreapta functioneaza\n");
    }else {
        printf("miscarea in dreapta nu functioneaza\n");
    }

    4. Cal
    Alb
    
    printf("Alb\n");
    if(validare_miscare_cal(board, 2, 2, 1, 4, 'A')) {
        printf("jos stanga pe nebun negru a mers\n");
    }else {
        printf("jos stanga pe nebun negru nu a mers\n");
    }

    if(validare_miscare_cal(board, 2, 2, 3, 4, 'A')) {
        printf("jos dreapta pe pion alb a mers\n");
    }else {
        printf("jos dreapta pe pion alb nu a mers\n");
    }

    if(validare_miscare_cal(board, 2, 2, 4, 1, 'A')) {
        printf("dreapta sus pe pion negru a mers\n");
    }else {
        printf("dreapta sus pe pion negru nu a mers\n");
    }

    if(validare_miscare_cal(board, 2, 2, 4, 3, 'A')) {
        printf("dreapta jos pe pion alb a mers\n");
    }else {
        printf("dreapta jos pe pion alb nu a mers\n");
    }

    printf("Negru\n");

    if(validare_miscare_cal(board, 2, 5, 3, 3, 'N')) {
        printf("sus dreapta pe tura alba mers\n");
    }else {
        printf("sus dreapta pe tura nu a mers\n");
    }

    if(validare_miscare_cal(board, 2, 5, 1, 3, 'N')) {
        printf("sus stanga pe regina neagra a mers\n");
    }else {
        printf("sus stanga pe regina nu a mers\n");
    }

    if(validare_miscare_cal(board, 2, 5, 4, 4, 'N')) {
        printf("dreapta sus pe pion negru a mers\n");
    }else {
        printf("dreapta sus pe pion negru nu a mers\n");
    }

    if(validare_miscare_cal(board, 2, 5, 4, 6, 'N')) {
        printf("dreapta jos pe pion alb a mers\n");
    }else {
        printf("dreapta jos pe pion alb nu a mers\n");
    }

    
    5. Nebun
    alb

    printf("Alb\n");
    if(validare_miscare_nebun(board, 3, 4, 0, 1, 'A') == true) {
        printf("miscare pe diagonala stanga sus si luarea calului negru merge\n");
    }else {
        printf("miscare pe diagonala stanga sus si luarea calului negru nu merge\n");
    }

    if(validare_miscare_nebun(board, 3, 4, 7, 0, 'A') == true) {
        printf("miscare pe diagonala dreapta sus si trecerea peste pionul negru merge\n");
    }else {
        printf("miscare pe diagonala dreapta sus si trecerea peste pionul negru nu merge\n");
    }

    if(validare_miscare_nebun(board, 3, 4, 0, 7, 'A') == true) {
        printf("miscare pe diagonala stanga jos merge\n");
    }else {
        printf("miscare pe diagonala stanga jos\n");
    }

    if(validare_miscare_nebun(board, 3, 4, 5, 6, 'A') == true) {
        printf("miscare pe diagonala dreapta jos si luarea peste pionul alb merge\n");
    }else {
        printf("miscare pe diagonala dreapta jos si luarea peste pionul alb nu merge\n");
    }

    printf("Negru\n");
    if(validare_miscare_nebun(board, 3, 3, 0, 0, 'N') == true) {
        printf("miscare pe diagonala stanga sus si luarea calului alb merge\n");
    }else {
        printf("miscare pe diagonala stanga sus si luarea calului alb nu merge\n");
    }

    if(validare_miscare_nebun(board, 3, 3, 6, 0, 'N') == true) {
        printf("miscare pe diagonala dreapta sus si trecerea peste pionul alb merge\n");
    }else {
        printf("miscare pe diagonala dreapta sus si trecerea peste pionul alb nu merge\n");
    }

    if(validare_miscare_nebun(board, 3, 3, 0, 6, 'N') == true) {
        printf("miscare pe diagonala stanga jos merge\n");
    }else {
        printf("miscare pe diagonala stanga jos\n");
    }

    if(validare_miscare_nebun(board, 3, 3, 7, 7, 'N') == true) {
        printf("miscare pe diagonala dreapta jos si trecerea peste calul negru merge\n");
    }else {
        printf("miscare pe diagonala dreapta jos si trecerea peste calul negru nu merge\n");
    }

    

    7. Rege
    alb
    
    printf("Alb\n");
    if(validare_micare_rege(board, 1, 4, 2, 3, 'A')) {
        printf("Miscare in dreapta sus peste pion alb merge\n");
    }else {
        printf("Miscare in dreapta sus peste pion alb nu merge\n");
    }

    if(validare_micare_rege(board, 1, 4, 1, 3, 'A')) {
        printf("Miscare in sus merge\n");
    }else {
        printf("Miscare in sus nu merge\n");
    }

    if(validare_micare_rege(board, 1, 4, 0, 3, 'A')) {
        printf("Miscare in stanga sus merge\n");
    }else {
        printf("Miscare in stanga sus nu merge\n");
    }

    if(validare_micare_rege(board, 1, 4, 0, 4, 'A')) {
        printf("Miscare in stanga merge\n");
    }else {
        printf("Miscare in stanga nu merge\n");
    }

    if(validare_micare_rege(board, 1, 4, 0, 5, 'A')) {
        printf("Miscare in stanga jos merge\n");
    }else {
        printf("Miscare in stanga jos merge\n");
    }

    if(validare_micare_rege(board, 1, 4, 1, 5, 'A')) {
        printf("Miscare in jos si luarea pionului negru merge\n");
    }else {
        printf("Miscare in jos si luarea pionului negru nu merge\n");
    }

    if(validare_micare_rege(board, 1, 4, 2, 5, 'A')) {
        printf("Miscare in dreapta jos merge\n");
    }else {
        printf("Miscare in dreapta jos nu merge\n");
    }

    if(validare_micare_rege(board, 1, 4, 2, 4, 'A')) {
        printf("Miscare in dreapta merge\n");
    }else {
        printf("Miscare in dreapta nu merge\n");
    }

    negru
    
    printf("Negru\n");
    if(validare_micare_rege(board, 5, 4, 6, 3, 'N')) {
        printf("Miscare in dreapta sus merge\n");
    }else {
        printf("Miscare in dreapta sus nu merge\n");
    }

    if(validare_micare_rege(board, 5, 4, 5, 3, 'N')) {
        printf("Miscare in sus merge\n");
    }else {
        printf("Miscare in sus nu merge\n");
    }

    if(validare_micare_rege(board, 5, 4, 4, 3, 'N')) {
        printf("Miscare in stanga sus peste cal negru merge\n");
    }else {
        printf("Miscare in stanga sus peste cal negru nu merge\n");
    }

    if(validare_micare_rege(board, 5, 4, 4, 4, 'N')) {
        printf("Miscare in stanga merge\n");
    }else {
        printf("Miscare in stanga nu merge\n");
    }

    if(validare_micare_rege(board, 5, 4, 4, 5, 'N')) {
        printf("Miscare in stanga jos merge\n");
    }else {
        printf("Miscare in stanga jos merge\n");
    }

    if(validare_micare_rege(board, 5, 4, 5, 5, 'N')) {
        printf("Miscare in jos si luarea pionului alb merge\n");
    }else {
        printf("Miscare in jos si luarea pionului alb nu merge\n");
    }

    if(validare_micare_rege(board, 5, 4, 6, 5, 'N')) {
        printf("Miscare in dreapta jos merge\n");
    }else {
        printf("Miscare in dreapta jos nu merge\n");
    }

    if(validare_micare_rege(board, 5, 4, 6, 4, 'N')) {
        printf("Miscare in dreapta merge\n");
    }else {
        printf("Miscare in dreapta nu merge\n");
    }

    */

    free_chess_board(board);
}