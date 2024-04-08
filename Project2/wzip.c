#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void decimalToBinaryBytes(int decimalValue, unsigned char *bytes) {
    for (int i = 0; i < 4; i++) {
        bytes[i] = (decimalValue >> (i * 8)) & 0xFF;  // Convert decimal to binary and store each byte (4 total) in array
    }
}

void writeToOutput(int count, char currentCharacter) {
    unsigned char bytes[5];
    bytes[4] = currentCharacter;
    decimalToBinaryBytes(count, bytes);
    fwrite(bytes, 1, 5, stdout);
}

void encodeRLE(FILE *inputFile) {
    char currentChar, nextChar;
    int count = 1;
    currentChar = getc(inputFile);  // Read first character
    // Loop through the file until EOF is reached
    while (currentChar != EOF) {
        nextChar = getc(inputFile); // Read Next Character
        if (nextChar == currentChar) {
            count++;
        } else {
            writeToOutput(count, currentChar); // Write Binary Output
            count = 1;  // Reset count for new character
        }
        currentChar = nextChar;  // Move to next character
    }
}

int main(int argc, char* args[]) {
    FILE* inputFile;
    
    if (argc == 1) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

    // If more than 1 file concatenate file's content together
    if (argc > 2) {
        FILE *tempFile = tmpfile();
        // Open each file path supplied and concatenate the contents to a temp file
        for (int file = 1; file < argc; file++) {
            inputFile = fopen(args[file], "rb"); 
            int inputCharacter;
            while ((inputCharacter = fgetc(inputFile)) != EOF) {
                fputc(inputCharacter, tempFile);  // Put each character in temp file
            }
        }
        
        rewind(tempFile);  // Put cursor at beginning of temp file

        encodeRLE(tempFile);  // Encode with combied file content
        fclose(tempFile);  // Close temp file which deletes it
    } else {
        inputFile = fopen(args[1], "rb");
        encodeRLE(inputFile);   
    }

    fclose(inputFile); // Close file
    return 0;
}