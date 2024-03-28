#include <stdio.h>
void otherCommands(char **stringArray, char *command, char **mainDirectory, int redirectionFlag, char *outputPath);
int processShell(FILE *fp);
int runCommand(char *givenLine, char **mainDirectory);
int findStringArrayLength(char **stringArray);
int addPaths(char** stringArray, char** mainDirectory, int stringArrayLength, int searchPathCount);
int runParallelCommands(char* currentLine, char** mainDirectory);


