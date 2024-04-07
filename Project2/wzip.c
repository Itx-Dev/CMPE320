#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void decimalToBinaryBytes(int decimalValue, unsigned char *bytes) {
    for (int i = 0; i < 4; i++) {
        bytes[i] = (decimalValue >> (i * 8)) & 0xFF;  // Convert decimal to binary and store each byte (4 total) in array
    }
}

void remove_newline(char *givenString) {
    int len = strlen(givenString);
    for (int i = 0; i < len; i++) {
        if (givenString[i] == '\n') {
            // Shift remaining characters to the left
            for (int j = i; j < len - 1; j++) {
                givenString[j] = givenString[j + 1];
            }
            givenString[len - 1] = '\0'; // Null-terminate the string
            len--; // Decrease length of string
            i--; // Adjust index to recheck current position
        }
    }
}


int checkNewLine(char* givenString) {
    if (givenString[strlen(givenString) - 1] == '\n') {
        remove_newline(givenString);
        return 1;
    } else {
        return 0;
    }
}

void encode(char* givenString) {
    char currentChar;
    int characterCount, copiedIndex;

    int checkNewLineFlag = checkNewLine(givenString);
        
    int stringLength = strlen(givenString);

    for (int i = 0; i < stringLength; i++) {
        characterCount = 1;
        while (i + 1 < stringLength && givenString[i] == givenString[i + 1]) {
            characterCount++;
            i++;
        }
        copiedIndex = i;
    }
    
    currentChar = givenString[copiedIndex - 1];

    unsigned char bytes[5];
    decimalToBinaryBytes(characterCount, bytes);
    bytes[4] = currentChar;

    if (checkNewLineFlag == 1) {
        char newLineArray[] = {1, 0, 0, 0, 10};
        fwrite(bytes, 1, 5, stdout);
        fwrite(newLineArray, 1, 5, stdout);
    } else {
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
        for (int i = 0; i < argc; i++) {
            filename = args[i];
            fp = fopen(filename, "r");
            if (getline(&currentLine, &lineLength, fp)) {
                strcat(concatenatedLine, currentLine);
            }
        }

        encode(concatenatedLine);
        fclose(fp);
    } else {
        // File name is already set from intialization
        fp = fopen(filename, "r");
        while (getline(&currentLine, &lineLength, fp) != -1) {
            encode(currentLine);
        }

        fclose(fp);
    }

    return 0;
}