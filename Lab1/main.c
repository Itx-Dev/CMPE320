#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    char tempArray[32][32];
    int occurancesCount[32];

    // Create array of strings without duplicates
    int count = 0;
        
    for (int currentWord = 0; currentWord < argc; currentWord++) {
        int occurances = 1;
        int occuranceIndex = 0;

        int targetWord;
        for (targetWord = 0; targetWord < count; targetWord++) {

            if (strcmp(argv[currentWord], argv[targetWord]) == 0) {
                ++occurances;
                break;
            }
        }
        if (targetWord == count) {
            strcpy(tempArray[count], argv[currentWord]);
            ++count;
        }

        occurancesCount[count - 1] = occurances;
        printf("%d\n", count);
    }

    for (int k = 0 ; k < count; k++) {
        printf("%s: %d\n", tempArray[k], occurancesCount[k]);
    }
}
