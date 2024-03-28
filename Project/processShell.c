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
void otherCommands(char **stringArray, char *command, char **mainDirectory, int redirectionFlag, char *outputPath) {
  int inputIndex = 0;
  // Discard output path from string array
  while (stringArray[inputIndex] != NULL) {
    if (strcmp(outputPath, stringArray[inputIndex]) == 0) {
      stringArray[inputIndex] = NULL;
    }
    inputIndex++;
  }

  int pathIndex = 0;
  int misses = 0;

  // Redirect output to file defined by user
  if (redirectionFlag > 0) {
    int outputFile = open(outputPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(outputFile, STDOUT_FILENO);
    close(outputFile);
  }

  while (mainDirectory[pathIndex] != NULL) {
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
  if (misses == pathIndex) { throwError(); }
  exit(0);
}

/**
 * @brief Find how many strings are in a string array
 *
 * @param stringArray
 * @return int
 */
int findStringArrayLength(char **stringArray) {
  // Find the length of the string array
  int stringArrayLength = 0;
  while (stringArray[stringArrayLength] != NULL) {
    // Remove new line char from each element of stringArray
    removeNewLine(stringArray[stringArrayLength]);
    stringArrayLength++;
  }
  return stringArrayLength;
}

int runCommand(char *givenLine, char **mainDirectory) {
  int searchPathCount = 64; // Define search path count for main directory
  char *originalString = malloc(searchPathCount * sizeof(char));
  strcpy(originalString, givenLine);  // make a copy of the lin
  char *outputPath = malloc(searchPathCount * sizeof(char *)), *inputString = malloc(searchPathCount * sizeof(char *));
  int redirectionFlag = searchForRedirection(originalString, &outputPath, &inputString);    // Returns index of redirection symbol (>), If redirection returns -1 if error
  if (redirectionFlag == -1)  { throwError(); return -1; } // If redirection flag error throw error and return
  char **stringArray = malloc(searchPathCount * sizeof(char *));
  stringArray = parseStringIntoArray(originalString, stringArray);  // Parse String into a string array
  originalString = NULL; free(originalString);  // Free Memory
  int stringArrayLength = findStringArrayLength(stringArray);  // Find number of strings in string array
  char *command = " ";  // Define command
  if (stringArray[0] != NULL) { command = stringArray[0]; } // Define Command
  if (strcmp(command, " ") == 0) { return -1; }  // if nothing is given try again
  if (strcmp(command, "exit") == 0) {     // If user exits
    if (stringArrayLength > 1) { throwError(); }  // Exit only takes 1 parameter
    else { exit(0); }
  }
  else if (strcmp(command, "cd") == 0) {  // If User decides to CD
    if (chdir(stringArray[1]) == 0) { } // This should be empty
    else { throwError(); } // Test for cd error
  }
  else if (strcmp(command, "path") == 0) {  // If User decides to PATH
    mainDirectory = clearDirectories(mainDirectory, searchPathCount); // Clear Directory
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
  // If user types any other command
  else {
    pid_t pid = fork();
    if (pid == 0) { otherCommands(stringArray, command, mainDirectory, redirectionFlag, outputPath); } // Child Performs
    else { waitpid(pid, NULL, 0); } // Parent Performs
  }
  for (int i = 0; i < stringArrayLength; i++) { stringArray[i] = NULL; free(stringArray[i]); } // Free Memory
  outputPath = NULL; free(outputPath); stringArray = NULL; free(stringArray); // Free Memory
}

int testBlankInput(const char *input) {
    while (*input != '\n') {
        if (*input != ' ') {
            return 0;
        }
        input++;
    }
    return 1;
}

/**
 * @brief Main shell loop
 * @param fp
 * @return int
 */
int processShell(FILE *fp) {
  int searchPathCount = 64;
  char *currentLine = NULL;
  char **mainDirectory = malloc(searchPathCount * sizeof(char *));
  size_t lineLength = 0, successfulGetLine;
  mainDirectory = clearDirectories(mainDirectory, searchPathCount);  // Reset searchPath so only built-in commands work
  mainDirectory[0] = "/bin";  // Set initial path
  while (1) {
    if (fp == stdin) { printf("wish> "); }     // Print prompt only if in batch mode
    successfulGetLine = getline(&currentLine, &lineLength, fp);    // Read in current Line
    if (successfulGetLine == -1) { exit(0); }     // If EOF is reached exit
    if (testBlankInput(currentLine) == 1) { continue; } // Handle Blank Input

    int parallelCommandsFlag = searchForParallelCommands(currentLine);    // Test for parallel Commands
    if (parallelCommandsFlag == -1) { return -1; }
    if (parallelCommandsFlag == 1) {
      char **parallelStringArray = malloc(32 * sizeof(char *));
      int commandIndex = 0;
      char *parallelCommand = strtok(currentLine, "&");       // Split line by &
      while (parallelCommand != NULL) {
        parallelStringArray[commandIndex] = parallelCommand;
        commandIndex++;
        parallelCommand = strtok(NULL, "&");
      }
      for (int i = 0; i < commandIndex; i++) {
        pid_t pid = fork();
        if (pid == 0) {
          runCommand(parallelStringArray[i], mainDirectory);
          exit(0);
        }
        else if (pid < 0) {
          throwError();
          exit(1);
        }
      }
      for (int i = 0; i < 3; i++) { wait(NULL); } // have parent wait for children
    }
    else {
      runCommand(currentLine, mainDirectory);
    }
  }
  currentLine = NULL; free(currentLine); mainDirectory = NULL; free(mainDirectory); // Free Memory
  return 0;
}