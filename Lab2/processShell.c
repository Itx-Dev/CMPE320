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
    size_t length = strlen(string);
    if (length > 0 && string[length - 1] == '\n') {
        string[length - 1] = '\0';
    }
}

/**
 * @brief Replace every string in string array with NULL
 * 
 * @param directory 
 * @param amountOfSearchPaths 
 * @return char** 
 */
char** clearDirectories(char** directory, int amountOfSearchPaths) {
    // Set entire array to NULL
    for (int i = 0; i < amountOfSearchPaths; i++) {
        directory[i] = NULL;
    }
    return directory;
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
    int index = 0;
    int searchPathCount = 32;
    char* currentLine = NULL;
    char** mainDirectory = malloc(searchPathCount * sizeof(char*));
    size_t lineLength = 0;
    size_t successfulGetLine;

    // Reset searchPath so only built-in commands work
    mainDirectory = clearDirectories(mainDirectory, searchPathCount);

    // Set initial path
    mainDirectory[0] = "/bin";

    while (1) {
        // Print prompt only if in batch mode
        if (fp == stdin) {
            printf("wish> ");
        }

        // Read in current Line
        successfulGetLine = getline(&currentLine, &lineLength, fp);
        // If comment in file skip
        if (currentLine[0] == '#') continue;

        // If EOF is reached exit
        if (successfulGetLine == -1) {
            exit(0);
        }
        
        // Create Space to store original string
        char* originalString = (char*)malloc(lineLength);
        strcpy(originalString, currentLine);

        removeNewLine(originalString);

        // Create Space to store command
        char** stringArray = malloc(32 * sizeof(char*));

        // Parse Command into string array
        stringArray = parseStringIntoArray(originalString, stringArray);

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
            // Exit only takes 1 parameter
            if (stringArrayLength > 1) {
                throwError();
            } else {
                exit(0);
            }
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
            if (stringArrayLength <= 1) {
                // Clear Directory and set default path
                mainDirectory = clearDirectories(mainDirectory, searchPathCount);
            } else {
                // Loop entire string array and add provided path to search 

                for (int stringArrayIndex = 1; stringArrayIndex < stringArrayLength; stringArrayIndex++) {
                    for (int directoryIndex = 0; directoryIndex < searchPathCount; directoryIndex++) {
                        if (mainDirectory[directoryIndex] == NULL && stringArray[stringArrayIndex] != NULL) {
                            mainDirectory[directoryIndex] = stringArray[stringArrayIndex];
                            break;
                        }
                    }
                }
            }
        } 

        // If user types any other command
        else {
            // If no path is given for ls use currentDirectory
            if (stringArray[1] == NULL && strcmp(command, "ls") == 0) {
                // Get current working directory
                char currentDirectory[120];
                getcwd(currentDirectory, sizeof(currentDirectory));
                stringArray[1] = currentDirectory;
            }

            // Check if directory is accessible for ls
            if (access(stringArray[1], F_OK) == -1 && strcmp(command, "ls") == 0) {
                fprintf(stderr, "%s: cannot access '%s': %s\n", command, stringArray[1], strerror(errno));
            } else {
                // Fork command process
                pid_t pid = fork();

                // Child process
                if (pid == 0) {
                    // Combine bin path to executable path
                    int pathIndex = 0;
                    int misses = 0;
                    while (mainDirectory[pathIndex] != NULL) {
                        char fullPath[100];
                        snprintf(fullPath, sizeof(fullPath), "%s/%s", mainDirectory[pathIndex], command);
                        // Define arguments
                        char *args[] = {fullPath, NULL};
                        // execv should not return if it does, error
                        if (execv(fullPath, args) == -1) {
                            misses++;
                        }
                        pathIndex++;
                    }
                    // If all directories fail throw error
                    if (misses == pathIndex) {
                        throwError();
                    }
                    exit(0);
                } else {
                    waitpid(pid, NULL, 0);
                }
            }
        }
        free(stringArray);
    }
    free(currentLine);
    free(mainDirectory);

    return 0;
}


