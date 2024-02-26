#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "error.h"

void otherCommands(char **stringArray, char *command, char **mainDirectory)
{
    // If no path is given for ls use currentDirectory
    if (stringArray[1] == NULL && strcmp(command, "ls") == 0)
    {
        // Get current working directory
        char currentDirectory[120];
        getcwd(currentDirectory, sizeof(currentDirectory));
        stringArray[1] = currentDirectory;
    }

    // Check if directory is accessible for ls
    if (access(stringArray[1], F_OK) == -1 && strcmp(command, "ls") == 0)
    {
        fprintf(stderr, "%s: cannot access '%s': %s\n", command, stringArray[1], strerror(errno));
    }
    else
    {
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
                char *args[] = {fullPath, NULL};
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
}