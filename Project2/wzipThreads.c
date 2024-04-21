#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <ctype.h>

#define THREADCOUNT 3

// Define structure for threads
typedef struct {
    struct arg_val_t {
        char* stringChunk;
    } arg_val; 
    struct ret_val_t {
		char* encodedString;
    } ret_val;
} arg_t;

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

void writeThreadOutput(char* rleString) {
    unsigned char bytes[5];

    // Parse the RLE-encoded string
    int len = strlen(rleString);
    int i = 0;
    while (i < len) {
        // Extract the count
        int count = atoi(&rleString[i]);
        // Move to the character
        while (rleString[i] >= '0' && rleString[i] <= '9') {
            i++;
        }
        // Extract the character
        char currentCharacter = rleString[i];
        bytes[4] = currentCharacter;

        // Write the count and character to output
        for (int j = 0; j < 4; j++) {
            bytes[j] = (count >> (j * 8)) & 0xFF;
        }
        fwrite(bytes, 1, 5, stdout);

        // Move to the next count or character
        i++;
    }
}

char* encodeThreadsRLE(char *inputString) {
    char currentChar, nextChar;
    int count = 1, outputIndex = 0;
    currentChar = inputString[0];  // Read first character

     char *encodedOutput = (char*)malloc((2 * strlen(inputString) + 1) * sizeof(char));

    // Loop through the string until the null terminator is reached
    for (int i = 1; inputString[i] != '\0'; i++) {
        nextChar = inputString[i]; // Read next character
        if (nextChar == currentChar) {
            count++;
        } else {
            outputIndex += sprintf(encodedOutput + outputIndex, "%d%c", count, currentChar);
            count = 1;  // Reset count for new character
        }
        currentChar = nextChar;  // Move to next character
    }
    outputIndex += sprintf(encodedOutput + outputIndex, "%d%c", count, currentChar);
    encodedOutput[outputIndex] = '\0';
    return encodedOutput;
}

void* threadFunction (void* arg) {
    arg_t* argStructs = (arg_t*)arg; // Take in thread structure
     // Get arguments from passed in parameter
    char* stringChunk = argStructs->arg_val.stringChunk;
    argStructs->ret_val.encodedString = encodeThreadsRLE(stringChunk);
    return NULL;
}

// Return file size in bytes
int getFileSize(FILE* file) {
	struct stat st;
	int fileDescriptor = fileno(file);
	fstat(fileDescriptor, &st);
	int fileSize = st.st_size;
	return fileSize;
}

int defineChunks(char *input, char *chunkNum, int chunkStartingIndex) {
    int length = strlen(input);
    int currentIndex = 0, done = 0;
    char previousChar;
    char currentChar = input[chunkStartingIndex];

    while (done == 0) {
        currentChar = input[chunkStartingIndex + currentIndex];
 
        if (currentIndex >= length / 3 && currentChar != previousChar) {
            done = 1;
            break;
        }

        if (chunkStartingIndex + currentIndex >= length) {
            done = 1;
            break;
        }

        chunkNum[currentIndex] = currentChar;

        previousChar = currentChar;

        currentIndex++;
    }
    chunkNum[chunkStartingIndex + currentIndex] = '\0';
    return (chunkStartingIndex + currentIndex);  // Ending index
}

int main(int argc, char* args[]) {
    FILE* inputFile;

    if (argc == 1) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

    if (argc > 2) {
        FILE* tempFile = tmpfile(); // Create temp file for single thread file concatenation
        for (int argIndex = 1; argIndex < argc; argIndex++) {
            inputFile = fopen(args[argIndex], "rb");  // Open File
            int fileSize = getFileSize(inputFile);  // Get file Size
            if (fileSize >= 4096) {
                    char* string = mmap(0, fileSize, PROT_READ, MAP_PRIVATE, fileno(inputFile), 0);	// Map file content to contiguous chunk of memory
                    int stringLength = strlen(string);
                    int sectionLength = stringLength / 3;

                    char* chunk1 = malloc(stringLength * sizeof(char));
                    char* chunk2 = malloc(stringLength * sizeof(char));
                    char* chunk3 = malloc(stringLength * sizeof(char));
                    
                    char* chunkArray[3] = {chunk1, chunk2, chunk3};

                    int chunk2StartingIndex = defineChunks(string, chunk1, 0);
                    int chunk3StartingIndex = defineChunks(string, chunk2, chunk2StartingIndex);
                    defineChunks(string, chunk3, chunk3StartingIndex);

                    pthread_t threads[THREADCOUNT];
                    arg_t threadParameter[THREADCOUNT];
                    
                    // Create threads, each responsible for processing a section of the string
                    for (int threadNum = 0; threadNum < THREADCOUNT; threadNum++) {
                            threadParameter[threadNum].arg_val.stringChunk = chunkArray[threadNum];
                            pthread_create(&threads[threadNum], NULL, threadFunction, (void*)&threadParameter[threadNum]);
                    }
                    // Wait for all threads to complete
                    for (int waitingThread = 0; waitingThread < THREADCOUNT; waitingThread++) {
                            pthread_join(threads[waitingThread], NULL);
                            writeThreadOutput(threadParameter[waitingThread].ret_val.encodedString);
                    }
            } else {
                // Open each file path supplied and concatenate the contents to a temp file
                int inputCharacter;
                while ((inputCharacter = fgetc(inputFile)) != EOF) {
                    fputc(inputCharacter, tempFile);  // Put each character in temp file
                }

                if (argIndex == argc - 1) {
                    rewind(tempFile);  // Put cursor at beginning of temp file
                    encodeRLE(tempFile);  // Encode with combied file content
                }
            }
        }
        fclose(tempFile);
    } else {
        inputFile = fopen(args[1], "rb");
        encodeRLE(inputFile);
    }
        return 0;
    }
