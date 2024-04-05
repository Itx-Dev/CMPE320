#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLENGTH 50
 
char count[5] = {'0'};
char* encode(char* givenString)
{
    int characterCount;
    // Find the occurances of a character
    for (int i = 0; i < strlen(givenString); i++) {
        // Count occurances of character
        characterCount = 1;
        while (i + 1 < strlen(givenString) && givenString[i] == givenString[i + 1]) {
            count[4] = givenString[i];
            characterCount++;
            i++;
        }
        // Put each character in count array
        sprintf(count, "%c", characterCount);
    }

    return count;
}
 
int main(int argc, char* args[])
{
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

    size_t lineLength = 0;
    size_t successfulLine;
    FILE *fp;

    char* currentLine = NULL;
    char* concatenatedLine = malloc(32 * sizeof(char));
    // Set first file name
    char* filename = args[1];

    // Encode concatenated Line
    if (argc > 2) {
        for (int i = 1; i < argc; i++) {
            // Change file name for multiple files
            filename = args[i];
            fp = fopen(filename, "r");
            // Get lines
            successfulLine = getline(&currentLine, &lineLength, fp);
            if (successfulLine != -1) {
                strcat(concatenatedLine, currentLine);
            }
        }

        char* response = encode(concatenatedLine);
        fwrite(response, 1, 5, stdout);
        fclose(fp);
    } 
    // Encode if there is only one file
    else 
    {
        fp = fopen(filename, "r");
        while (getline(&currentLine, &lineLength, fp) != -1) {
            char *response = encode(currentLine);
            fwrite(response, 1, 5, stdout);
        }
        fclose(fp);
    }

    return 0;    
}