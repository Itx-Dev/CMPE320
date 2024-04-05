#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAXLENGTH 50
 
char count[5] = {'0'};
int numberOfCharacters = 0;

char* encode(char* givenString)
{
    int characterCount;
    // Find the occurances of a character
    for (int i = 0; i < strlen(givenString); i++) {
        count[4] = givenString[i];
        // Count occurances of character
        characterCount = 1;
        while (i + 1 < strlen(givenString) && givenString[i] == givenString[i + 1]) {
            characterCount++;
            i++;
        }
        sprintf(count, "%c", characterCount);
    }
    numberOfCharacters++;

    return count;
}
 
int main(int argc, char* args[])
{
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

    size_t lineLength = 0;
    size_t successfulGetLine;

    char* currentLine = NULL;
    char* concatenatedLine = malloc(32 * sizeof(char));

    for (int i = 1; i < argc; i++) {
        char* filename = (char*)args[i];

        FILE* fp = fopen(filename, "r");
        
        successfulGetLine = getline(&currentLine, &lineLength, fp);    // Read in current Line
        strcat(concatenatedLine, currentLine);

        if (successfulGetLine == -1) {
            return 1;
        }
    }

    char* response = encode(concatenatedLine);

    for (int i = 0; i < 5; i++) {
        printf("%c", response[i]);
    }
    return 0;    
}