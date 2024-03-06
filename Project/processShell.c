#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#include "processShell.h"
#include "utilities.h"

/**
 * @brief Algorithm for any command that is not built into processShell
 * 
 * @param stringArray 
 * @param command 
 * @param mainDirectory 
 */
void otherCommands(char **stringArray, char *command, char **mainDirectory, int redirectionFlag, char* outputPath)
{
    int inputIndex = 0;
    while (stringArray[inputIndex] != NULL) {
        if (strcmp(outputPath, stringArray[inputIndex]) == 0) {
            stringArray[inputIndex] = NULL;
        }
        inputIndex++;
    }

    // Fork command process
    pid_t pid = fork();
    // Child process
    if (pid == 0)
    {
        // Combine bin path to executable path
        int pathIndex = 0;
        int misses = 0;


        if (redirectionFlag > 0) {
            int outputFile = open(outputPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(outputFile, STDOUT_FILENO);
            close(outputFile);
        }
        while (mainDirectory[pathIndex] != NULL)
        {
            char fullPath[100];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", mainDirectory[pathIndex], command);
            // Define arguments
            // execv should not return if it does, error
            if (execv(fullPath, stringArray) == -1)
            {
                misses++;
            }
            pathIndex++;
        }
        // If all directories fail throw error
        if (misses == pathIndex)
        {
            throwError();
        }
        exit(0);
    }
    else
    {
        waitpid(pid, NULL, 0);
    }
}

/**
 * @brief Main shell loop
 * 
 * @param fp 
 * @return int 
 */
int processShell(FILE* fp) {
    int index = 0;
    int searchPathCount = 64;
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
        removeNewLine(currentLine);
        // If EOF is reached exit
        if (successfulGetLine == -1) {
            exit(0);
        }

        // Copy string to new string for manipulation through redirection 
        char* copyForRedirection = malloc(searchPathCount * sizeof(char));
        strcpy(copyForRedirection, currentLine);
        
        char* outputPath = malloc(searchPathCount * sizeof(char*));
        char* inputString = malloc(searchPathCount * sizeof(char*));
        // Returns index of redirection symbol (>)
        int redirectionFlag = searchForRedirection(copyForRedirection, &outputPath, &inputString);
        // If redirection returns -1 if error
        if (redirectionFlag == -1) {
            throwError();
            continue;
        }
        // Free Memory
        copyForRedirection = NULL; free(copyForRedirection);
        
        // Create Space to store original string
        char* originalString = malloc(searchPathCount * sizeof(char));
        strcpy(originalString, currentLine);

        // Create Space to store command
        char** stringArray = malloc(searchPathCount * sizeof(char*));

        // Parse Command into string array
        stringArray = parseStringIntoArray(originalString, stringArray);

        // Free Memory
        originalString = NULL; free(originalString);

        // Find the length of the string array
        int stringArrayLength = 0;
        while (stringArray[stringArrayLength] != NULL) {
            // Remove new line char from each element of stringArray
            removeNewLine(stringArray[stringArrayLength]);
            stringArrayLength++;
        }

        // Define command
        char* command = stringArray[0];

        // BUILT IN COMMANDS 

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
            }
        }

        // If User decides to PATH
        else if (strcmp(command, "path") == 0) {
            if (stringArrayLength <= 1)
            {
                // Clear Directory and set default path
                mainDirectory = clearDirectories(mainDirectory, searchPathCount);
            }
            else
            {
                // Loop entire string array and add provided path to search

                for (int stringArrayIndex = 1; stringArrayIndex < stringArrayLength; stringArrayIndex++)
                {
                    for (int directoryIndex = 0; directoryIndex < searchPathCount; directoryIndex++)
                    {
                        if (mainDirectory[directoryIndex] == NULL && stringArray[stringArrayIndex] != NULL)
                        {
                            mainDirectory[directoryIndex] = stringArray[stringArrayIndex];
                            break;
                        }
                    }
                }
            }
        } 

        // If user types any other command
        else {
            otherCommands(stringArray, command, mainDirectory, redirectionFlag, outputPath);
        }
        // Free Memory
        for (int i = 0; i < stringArrayLength; i++) {
            stringArray[i] = NULL; free(stringArray[i]);
        }
        outputPath = NULL; free(outputPath);
        stringArray = NULL; free(stringArray);
    }
    currentLine = NULL;
    mainDirectory = NULL;
    free(currentLine);
    free(mainDirectory);

    return 0;
}