#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {    
    int count = 0;

    while(1) {
        if(count == 0) {
            count++;
            continue;
        }

        printf("Image\n");
        break;
    }
    return 0;
}