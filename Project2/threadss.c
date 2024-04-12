#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

// Define a mutex for synchronization
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    char* rawString;
    int start;  // Start index of the section
    int length; // Length of the section
    char* result; // Pointer to store the result
} ThreadArgs;

void* threadFunction(void* arg) {
    ThreadArgs* threadArgs = (ThreadArgs*)arg;
    char* rawString = threadArgs->rawString;
    int start = threadArgs->start;
    int length = threadArgs->length;

    // Allocate memory for the result
    threadArgs->result = (char*)malloc((length + 1) * sizeof(char));
    if (threadArgs->result == NULL) {
        perror("Memory allocation failed");
        pthread_exit(NULL);
    }

    // Lock the mutex before accessing shared resources
    pthread_mutex_lock(&mutex);

    // Copy the section of the string into the result buffer
    strncpy(threadArgs->result, rawString + start, length);
    threadArgs->result[length] = '\0'; // Null-terminate the string

    // Unlock the mutex after accessing shared resources
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void decimalToBinaryBytes(int decimalValue, unsigned char *bytes) {
	for (int i = 0; i < 4; i++) {
		bytes[i] = (decimalValue >> (i * 8)) & 0xFF; // Convert decimal to binary and store each byte (4 total) in array
	}
}

void writeToOutput(int count, char currentCharacter) {
	unsigned char bytes[5];
	bytes[4] = currentCharacter;
	decimalToBinaryBytes(count, bytes);
	fwrite(bytes, 1, 5, stdout);
}

void encodeRLE(char* inputString) {
    char currentChar, nextChar;
    int count = 1;
    currentChar = *inputString; // Read first character

    // Loop through the string until NULL terminator is reached
    while (currentChar != '\0') {
        nextChar = *(inputString + 1); // Read Next Character

        if (nextChar == currentChar) {
            count++;
        } else {
            writeToOutput(count, currentChar); // Write Binary Output

            count = 1;  // Reset count for new character
        }

        inputString++; // Move to next character
        currentChar = *inputString;
    }
}

int main(int argc, char* args[]) {
    FILE* inputFile;


    if (argc > 2) {
        inputFile = fopen(args[2], "rb");
        int fd = fileno(inputFile);
        struct stat st;
        fstat(fd, &st);
        int fsize = st.st_size;
        char* string = mmap(0, fsize, PROT_READ, MAP_PRIVATE, fd, 0);
        int stringLength = strlen(string);
        int numOfThreads = 3;
        int sectionLength = stringLength / numOfThreads;
        
        

        pthread_t threads[numOfThreads];
        ThreadArgs threadArgs[numOfThreads];

        // Create threads, each responsible for printing a section of the string
        for (int threadNum = 0; threadNum < numOfThreads; threadNum++) {
            threadArgs[threadNum].rawString = string;
            threadArgs[threadNum].start = threadNum * sectionLength;
            threadArgs[threadNum].length = (threadNum == numOfThreads - 1) ? stringLength - threadArgs[threadNum].start : sectionLength;
            pthread_create(&threads[threadNum], NULL, threadFunction, (void*)&threadArgs[threadNum]);
        }

        // Wait for all threads to complete
        for (int waitingThread = 0; waitingThread < numOfThreads; waitingThread++) {
            pthread_join(threads[waitingThread], NULL);
        }

        // Concatenate the results
        char* mergedResult = (char*)malloc((stringLength + 1) * sizeof(char));
        if (mergedResult == NULL) {
            perror("Memory allocation failed");
            return 1;
        }

        // Copy the results of each thread into the mergedResult buffer
        int offset = 0;
        for (int i = 0; i < numOfThreads; i++) {
            strcpy(mergedResult + offset, threadArgs[i].result);
            offset += strlen(threadArgs[i].result);
            free(threadArgs[i].result); // Free memory allocated for each thread's result
        }
        mergedResult[offset] = '\0'; // Null-terminate the merged result

        encodeRLE(mergedResult);

        // Free memory allocated for the merged result
        free(mergedResult);
    }

    return 0;
}