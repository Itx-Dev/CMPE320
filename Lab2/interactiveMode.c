#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int interactiveMode(int argc, char* args[]) {
    char* currentLine = NULL;
    size_t lineLength = 0;
    size_t read;
    int index = 1;
    while (1) {
        printf("wish> ");
        read = getline(&currentLine, &lineLength, stdin);
        if (strcmp(currentLine, "exit") == 10){
            exit(0);
        }
        printf("Line %d: %s", index, currentLine);
        index++;
    }
}