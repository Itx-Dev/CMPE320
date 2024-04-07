#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void decimalToBinaryBytes(int decimalValue, unsigned char *bytes) {
    for (int i = 0; i < 4; i++) {
        bytes[i] = (decimalValue >> (i * 8)) & 0xFF;  // Convert decimal to binary and store each byte (4 total) in array
    }
}

void encode(char* givenString) {
    char currentChar;
    int characterCount;
        
    int stringLength = strlen(givenString);

    for (int i = 0; i < stringLength; i++) {
        unsigned char bytes[5] = {0};
        characterCount = 1;
        while (i + 1 < stringLength && givenString[i] == givenString[i + 1]) {
            characterCount++;
            i++;
        }

        currentChar = givenString[i];

        decimalToBinaryBytes(characterCount, bytes);
        bytes[4] = currentChar;

        fwrite(bytes, 1, 5, stdout);
    }

    
}

int main(int argc, char* args[])
{
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

    size_t lineLength = 0;
    FILE *fp;

    char* currentLine = NULL;
    char* concatenatedLine = malloc(32 * sizeof(char));
    char* filename = args[1];

    // If more than 1 file concatenate file's content together
    if (argc > 2) {
        for (int i = 1; i < argc; i++) {
            filename = args[i];
            fp = fopen(filename, "r");
            if (getline(&currentLine, &lineLength, fp) != -1) {
                strcat(concatenatedLine, currentLine);
            }
        }
        encode(concatenatedLine);
        concatenatedLine = NULL; free(concatenatedLine);
        fclose(fp);
    } else {
        // File name is already set from intialization
        fp = fopen(filename, "r");
        while (getline(&currentLine, &lineLength, fp) != -1) {
            encode(currentLine);
        }

        fclose(fp);
    }

    currentLine = NULL; free(currentLine);
    filename = NULL; free(filename);

    return 0;
}