#include <stdlib.h>
#include <stdio.h>

int batchMode(int argc, char* args[]) { 
    char* currentLine = NULL;
    size_t lineLength = 0;
    size_t read;

    for (int i = 1; i < argc; i++) {
        FILE *file = fopen(args[i], "r");
        int index = 1;
        while ((read = getline(&currentLine, &lineLength, file)) != -1) {
            printf("Line %d: %s", index, currentLine);
            index++;
        }
        printf("\n");
        exit(0);
    }
}
