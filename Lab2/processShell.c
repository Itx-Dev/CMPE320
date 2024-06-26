#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "error.h"
#include "processShell.h"
#include "otherCommands.h"
#include "utilities.h"

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
        removeNewLine(currentLine);

         // Create Space to store original string
        char* copyForRedirection = (char*)malloc(64 * sizeof(char));
        strcpy(copyForRedirection, currentLine);
        
        char* outputPath = NULL;
        char* inputString = NULL;

        if (searchForRedirection(copyForRedirection, &outputPath, &inputString) != -1) {
            FILE* redirectionPTR = fopen(outputPath, "w+");
            fprintf(redirectionPTR, "%s", inputString);
            // Skip to next iteration
            fclose(redirectionPTR);
            redirectionPTR = NULL;
            free(outputPath); free(inputString); free(copyForRedirection);
            continue;
        }

        // Free Memory
        copyForRedirection = NULL; free(copyForRedirection);

        // If EOF is reached exit
        if (successfulGetLine == -1) {
            exit(0);
        }
        
        // Create Space to store original string
        char* originalString = (char*)malloc(lineLength);
        strcpy(originalString, currentLine);

        // Create Space to store command
        char** stringArray = malloc(32 * sizeof(char*));

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
            otherCommands(stringArray, command, mainDirectory);
        }
        stringArray = NULL;
        free(stringArray);
    }
    currentLine = NULL;
    mainDirectory = NULL;
    free(currentLine);
    free(mainDirectory);

    return 0;
}