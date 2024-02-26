#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "error.h"

void otherCommands(char **stringArray, char *command, char **mainDirectory)
{
    char* currentDirectory = stringArray[1];
    // If no path is given for ls use currentDirectory
    if (stringArray[1] == NULL && strcmp(command, "ls") == 0)
    {
        getcwd(currentDirectory, sizeof(currentDirectory));
        stringArray[1] = currentDirectory;
    }

    // Fork command process
    pid_t pid = fork();

    // Child process
    if (pid == 0)
    {
        // Combine bin path to executable path
        int pathIndex = 0;
        int misses = 0;
        while (mainDirectory[pathIndex] != NULL)
        {
            char fullPath[100];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", mainDirectory[pathIndex], command);
            // Define arguments
            char *args[] = {command, currentDirectory, NULL};
            // execv should not return if it does, error
            if (execv(fullPath, args) == -1)
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