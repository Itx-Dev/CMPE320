#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "error.h"

/**
 * @brief Remove new line character from string.
 * @param string 
 */
void removeNewLine(char *string) {
    if (string[strlen(string) - 1] == '\n') {
        string[strlen(string) - 1] = '\0';
    }
}

/**
 * @brief Break given command down into an array of strings and return the array of strings
 * 
 * @param givenLine 
 * @param storageArray 
 * @return char** Array of Strings with current line
 */
char** parseStringIntoArray(char *givenLine, char** storageArray) {
    char* splitString = NULL;
    int argumentIndex = 0;

    // Split String into tokens by spaces
    splitString = strtok(givenLine, " ");
    removeNewLine(splitString);

    // Parse string given by user or batch file and split into string array
    while (splitString != NULL) {

        // Store split string into a string array
        storageArray[argumentIndex] = splitString;

        // Test for NULL pointer
        splitString = strtok(NULL, " ");
        argumentIndex++;
    }

    return storageArray;
}

int processShell(FILE* fp) {
    char* currentLine = NULL;
    char* mainDirectory = "/bin";
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

        // Parse Command into string array
        stringArray = parseStringIntoArray(currentLine, stringArray);

        // Find the length of the string array
        int stringArrayLength = 0;
        while (stringArray[stringArrayLength] != NULL) {
            // Remove new line char from each element of stringArray
            removeNewLine(stringArray[stringArrayLength]);
            stringArrayLength++;
        }

        // Define command
        char* command = stringArray[0];

        // If User decides to Exit
        if (strcmp(command, "exit") == 0){
            exit(0);
        }

        // If User decides to CD
        else if (strcmp(command, "cd") == 0) {

            if( chdir( stringArray[1] ) == 0 ) {
                // This should be empty
            } 
            // Test for cd error
            else {
                throwError();
                return -1;
            }



        }

        // If User decides to PATH
        else if (strcmp(command, "path") == 0) {

        } else {
            // Get current working directory
            char currentDirectory[1024];
            getcwd(currentDirectory, sizeof(currentDirectory));

            if (stringArray[1] == NULL) {
                stringArray[1] = currentDirectory;
            }

            // Check if directory is accessible
            if (access(stringArray[1], F_OK) == -1) {
                fprintf(stderr, "%s: cannot access '%s': %s\n", command, stringArray[1], strerror(errno));
            } else {
                // Fork command process
                int pid = fork();
                // Child process
                if (pid == 0) {
                    // Combine bin path to executable path
                    char fullPath[100];
                    snprintf(fullPath, sizeof(fullPath), "%s/%s", mainDirectory, command);

                    // Define arguments
                    char *args[] = {fullPath, NULL};
                    execv(fullPath, args);
                }
            }
        }
    }

}


