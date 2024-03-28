int throwError();
int removeNewLine(char *string);
int removeTrailingSpaces(char *str);
int removeSpaces(char *str);
int testBlankInput(char *input);
int searchForRedirection(char *readInLine, char **outputString, char **inputString);
int searchForParallelCommands(char *readInLine);
char **clearDirectories(char **directory, int amountOfSearchPaths);
char **parseStringIntoArray(char *givenLine, char **storageArray);
