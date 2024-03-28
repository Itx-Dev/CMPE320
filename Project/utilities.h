int throwError();
int searchForRedirection(char* readInLine, char** outputString, char** inputString);
int searchForParallelCommands(char* readInLine);
char* removeTrailingSpaces(char* givenString);
void removeSpaces(char* str);
int removeNewLine(char* string);
char** clearDirectories(char** directory, int amountOfSearchPaths);
char** parseStringIntoArray(char* givenLine, char** storageArray);