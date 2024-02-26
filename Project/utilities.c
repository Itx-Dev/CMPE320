#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


int throwError() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

/**
 * @brief Removes spaces in a string
 * 
 * @param readInLine 
 */
int removeSpaces(char* readInLine) {
    int noSpaceStringIndex = 0;
    // Remove spaces in string
    for (int i = 0; readInLine[i] != '\0'; i++)
    {
        // If character is not a space, put character into new string 
        if (readInLine[i] != ' ')
        {
            readInLine[noSpaceStringIndex] = readInLine[i];
            noSpaceStringIndex++;
        }    
    }
    // Set NULL terminator to end of string
    readInLine[noSpaceStringIndex] = '\0';
    return 0;
}

/**
 * @brief Remove new line character from string.
 * @param string 
 */
int removeNewLine(char* string) {
    size_t length = strlen(string);
    if (length > 0 && string[length - 1] == '\n') {
        string[length - 1] = '\0';
    }
    return 0;
}

/**
 * @brief Searches for redirection symbol (>)
 * @param outputString, the address of a pointer where to store the value of the string after the (>) symbol
 * @param inputString, the address of a pointer where to store the value of the string before the (>) symbol
 * @param readInLine, the current line read in by the shell
 * @return int, -1 if no (>) symbol found, else the index of the symbol 
 */
int searchForRedirection(char* readInLine, char** outputString, char** inputString) {
    // Remove spaces in line read in
    removeNewLine(readInLine);
    removeSpaces(readInLine);

    int redirectionIndex = -1;
    int redirectionBoolean = 0;
    int outputIndex = 0;
    int inputIndex = 0;
    char* inputPath = malloc(32 * sizeof(char));
    char* outputPath = malloc(32 * sizeof(char));
    // Find Redirection Symbol
    for (int i = 0; readInLine[i] <= strlen(readInLine); i++) {
        // Must stay above > detection
        // Update new string with every character after the redirection symbol
        if (redirectionBoolean == 1) {
            outputPath[outputIndex] = readInLine[i];
            outputIndex++;
        }

        // If redirection symbol found set index and set bool value to true
        if (readInLine[i] == '>') {
            redirectionIndex = i;
            redirectionBoolean = 1;
        }

        // Store String before (>) Keep after reflection symbol check
        if (redirectionBoolean == 0) {
            inputPath[inputIndex] = readInLine[i];
            inputIndex++;
        }
    }

    // Add null terminator
    inputPath[inputIndex] = '\0';
    outputPath[outputIndex] = '\0';

    if (redirectionBoolean == 1) {
        if (strlen(inputPath) == 0 || strlen(outputPath) == 0) {
            throwError();
            return -1;
        }
    }


    // Update string passed in through parameter
    *inputString = inputPath;
    *outputString = outputPath;

    // Free memory
    inputPath = NULL; outputPath = NULL;
    free(inputPath); free(outputPath);

    return redirectionIndex;
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