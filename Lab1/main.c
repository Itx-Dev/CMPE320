#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    // Length of arguments
    int length = argc;
    char usedWords[32][15];
    char tempArray[32][15];

    // Initialize Array to NULL Terminator
    for (int row = 0; row < 32; row++) {
        for (int col = 0; col < 15; col++) {
            usedWords[row][col] = '\0';
            tempArray[row][col] = '\0';
        }
    }

    for (int currentWordIndex = 1; currentWordIndex < length; currentWordIndex++) {
        // Reset occurances every target word change
        int occurances = 0;

        char* currentWord = argv[currentWordIndex];
 
        // Word to find
        for (int targetWordIndex = 1; targetWordIndex < length; targetWordIndex++) {
            char* targetWord = argv[targetWordIndex];

            // If word matches argument
            if (strcmp(currentWord, targetWord) == 0) {
                occurances++;
            }
         }
        // Copy used word to new array
        strcpy(usedWords[currentWordIndex - 1], argv[currentWordIndex]);

    }

    // Create array of strings without duplicates
    int count = 0;
        
    for (int i = 0; i < length - 1; i++) {
        int j;
        for (j = 0; j < count; j++) {
            if (strcmp(usedWords[i], tempArray[j]) == 0) {
                break;
            }
        }
        if (j == count) {
            strcpy(tempArray[count], usedWords[i]);
            count++;
        }
    }

    for (int k = 0 ; k < count; k++) {
        printf("%s: %d\n", tempArray[k], count);
    }
}
