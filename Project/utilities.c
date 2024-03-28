#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

int throwError() {
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}

/**
 * @brief Break given command down into an array of strings and return the array of strings
 *
 * @param givenLine
 * @param storageArray
 * @return char** Array of Strings with current line
 */
char **parseStringIntoArray(char *givenLine, char **storageArray) {
  char *splitString = malloc(32 * sizeof(char));
  int argumentIndex = 0;

  splitString = strtok(givenLine, " \t>");  // Split String into tokens by spaces

  // Parse string given by user or batch file and split into string array
  while (splitString != NULL) {
    storageArray[argumentIndex] = splitString;    // Store split string into a string array

    splitString = strtok(NULL, " \t>");    // Test for NULL pointer
    argumentIndex++;
  }

  return storageArray;
}

/**
 * @brief Remove new line character from string.
 * @param string
 */
int removeNewLine(char *string) {
  size_t length = strlen(string);
  if (length > 0 && string[length - 1] == '\n') { string[length - 1] = '\0'; }
  return 0;
}

// Function to remove trailing spaces from a string
void removeTrailingSpaces(char *str) {
  int i;
  // Find the index of the last non-space character
  for (i = strlen(str) - 1; i >= 0; i--) {
    if (str[i] != ' ') { break; }
  }
  str[i + 1] = '\0';  // Null-terminate the string at the last non-space character
}

// Function to remove spaces from a string
void removeSpaces(char *str) {
  int i, j = 0;

  // Iterate through the string
  for (i = 0; i < strlen(str); i++) {
    // If the current character is not a space, copy it to the new position
    if (str[i] != ' ') { str[j++] = str[i]; }
  }
  str[j] = '\0';  // Null-terminate the string at the new position
}

int searchForParallelCommands(char *readInLine) {
  int ampersandCount = 0, commandIndex = 0;
  // Look for ampersand
  for (int i = 0; i < strlen(readInLine); i++) {
    if (readInLine[i] == '&') {
      if (i == 0) { return -1; } // if string starts with ampersand return error code
      else { ampersandCount++; } // else increment ampersand counter
    }
  }


  if (ampersandCount == 0) { return 0; }  // return 0 if ampersand is not found
  else if (ampersandCount > 0) { return 1; } // return 1 if ampersand is found
}

/**
 * @brief Searches for redirection symbol (>)
 * @param outputString, the address of a pointer where to store the value of the string after the (>) symbol
 * @param inputString, the address of a pointer where to store the value of the string before the (>) symbol
 * @param readInLine, the current line read in by the shell
 * @return int, 0 if no (>) symbol found, -1 if error occurs, else the index of the symbol
 */
int searchForRedirection(char *readInLine, char **outputString, char **inputString) {
  int redirectionIndex = 0, redirectionBoolean = 0, pathIndex = 0, argIndex = 0;
  char *input = malloc(32 * sizeof(char)), *output = malloc(32 * sizeof(char));
  removeNewLine(readInLine);  // Remove spaces in line read in
  removeTrailingSpaces(readInLine); // Remove Trailing Spaces
  // Find Redirection Symbol
  for (int i = 0; i <= strlen(readInLine) - 1; i++) {
    if (readInLine[i] == '>') {    // If redirection symbol found set index and set bool value to true
      if (redirectionBoolean == 1) {     
        throwError();
        return -1;
      }
      redirectionIndex = i; // Set Index of redirection symbol
      redirectionBoolean = 1;      // Set that (>) was found
      pathIndex = 0; // Reset index to store args after (>) symbol
    }
    // Store command before (>) symbol
    else if (redirectionBoolean == 0) {
      input[pathIndex] = readInLine[i];      // Store string
      pathIndex++;
    }

    else if (redirectionBoolean == 1) {    // Store args after (>) symbol
      if (readInLine[i] == ' ' && strlen(output) != 0) { return -1; } // If the first directed output file string ends and there is still more characters
      output[pathIndex] = readInLine[i];
      pathIndex++;
    }
  }

  // If > found but no output given throw error
  if (redirectionBoolean == 1 && strlen(output) == 0) { return -1; }
  removeSpaces(output);
  *inputString = input; *outputString = output;  // Update string passed in through parameter
  input = NULL;  free(input); output = NULL;  free(output);  // Free memory
  return redirectionIndex;
}

/**
 * @brief Replace every string in string array with NULL
 *
 * @param directory
 * @param amountOfSearchPaths
 * @return char**
 */
char **clearDirectories(char **directory, int amountOfSearchPaths) {
  // Set entire array to NULL
  for (int i = 0; i < amountOfSearchPaths; i++) { directory[i] = NULL; }
  return directory;
}