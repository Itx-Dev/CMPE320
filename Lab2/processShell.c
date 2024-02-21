#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int processShell(FILE* fp) {
    char* currentLine = NULL;
    size_t lineLength = 0;
    size_t read;
    int index = 1;

    while (read != EOF) {
        printf("wish> ");
        // Read in current Line
        read = getline(&currentLine, &lineLength, fp);

        // If User decides to Exit
        if (strcmp(currentLine, "exit") == 0){
            break;
        } 
        // If User decides to CD
        else if (strcmp(currentLine, "cd") == 0) {
            for (int i = 2; i < strlen(currentLine); i++) {
                printf("%c", currentLine[i]);
            }
        } 
        // If User decides to PATH
        else if (strcmp(currentLine, "path") == 0) {
            printf("5");
        }

        printf("Line %d: %s", index, currentLine);
        index++;
    }
    //printf("\n");
    exit(0);
}