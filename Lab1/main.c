#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(int argc, char* argv[]) {
    // Dynamically allocate string array for size of arguments
    char** usedWords;
    usedWords = malloc(argc * sizeof(char*));

    // Allocate space for argc number of elements
    for (int i = 0; i < argc; i++) {
        usedWords[i] = malloc(strlen(argv[i]) * sizeof(char));
    }

    // Assert dynamic string array is not empty
    assert(usedWords != NULL);

    int occurances;
    int uniqueStrings = 0;
    int printIndex = 0;

    // Loop over argument array tracking unique strings and counting occurances
    for (int currentWordIndex = 0; currentWordIndex < argc; currentWordIndex++) {
        occurances = 0;

        char* currentWord = argv[currentWordIndex];

        for (int usedWordIndex = 0; usedWordIndex < argc; usedWordIndex++) {
            // If usedWords string at index matches then duplicate and skip.
            if (strcmp(currentWord, usedWords[usedWordIndex]) == 0 && usedWordIndex != currentWordIndex) {
                break;
            }
            // else if current word does not match and the space in used words is empty, copy to that index 
            else if (strcmp(usedWords[usedWordIndex], "") == 0) {
                strcpy(usedWords[usedWordIndex], currentWord);
                // Keep track of how many strings are unique
                uniqueStrings++;
                break;
            }
        }

        // Count occurances of each argument
        for (int targetWordIndex = 0; targetWordIndex < argc; targetWordIndex++) {
            if ((strcmp(argv[targetWordIndex], currentWord)) == 0) {
                occurances++;
            }
        }

        // Print final output
        while (strcmp(usedWords[printIndex], "") != 0) {
            printf("%s: %d\n", usedWords[printIndex], occurances);
            printIndex++;
        }
    }
}
