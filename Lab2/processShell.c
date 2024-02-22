#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "error.h"

/**
 * @brief Remove new line character from string.
 * @param string 
 */
void removeNewLine(char* string) {
    if (string[strlen(string) - 1] == '\n') {
        string[strlen(string) - 1] = '\0';
    }
}

int processShell(FILE* fp) {
    char* currentLine = NULL;
    char* splitString = NULL;
    size_t lineLength = 0;
    size_t read;
    int index = 0;

    while (1) {
        // Print prompt only if in batch mode
        if (fp == stdin) {
            printf("wish> ");
        }

        // Read in current Line
        read = getline(&currentLine, &lineLength, fp);

        // If EOF is reached exit
        if (read == -1) {
            exit(0);
        }
        
        // Create Space to store original string
        char* originalString = (char*)malloc(lineLength);
        strcpy(originalString, currentLine);
        removeNewLine(originalString);


        // Create Space to store command
        char** stringArray = malloc(32 * sizeof(char*));

        int argumentIndex = 0;

        // Split String into tokens by spaces
        splitString = strtok(currentLine, " ");
        removeNewLine(splitString);

        // Parse string given by user or batch
        while (splitString != NULL) {

            // Store split string into a string array
            stringArray[argumentIndex] = splitString;


            // If User decides to Exit
            if (strcmp(stringArray[0], "exit") == 0){
                exit(0);
            }

            // If User decides to CD
            else if (strcmp(stringArray[0], "cd") == 0) {
                // Check for cd error
                if (stringArray[1] == NULL) {
                    throwError();
                    return -1;
                }
            }

            // If User decides to PATH
            else if (strcmp(stringArray[0], "path") == 0) {
                exit(0);
            }

            // Test for NULL pointer
            splitString = strtok(NULL, " ");
            argumentIndex++;
        }

        // Print results
        index++;
        printf("Line %d: %s\n", index, originalString);
    }
}


