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
} ThreadArgs;

void* threadFunction(void* arg) {
    ThreadArgs* threadArgs = (ThreadArgs*)arg;
    char* rawString = threadArgs->rawString;
    int start = threadArgs->start;
    int length = threadArgs->length;

    char currentChar, nextChar;
    int count = 1;
    currentChar = *(rawString + start); // Read first character

    // Loop through the section until its end
    for (int i = start + 1; i < start + length; i++) {
        nextChar = *(rawString + i); // Read Next Character

        if (nextChar == currentChar) {
            count++;
        } else {
            pthread_mutex_lock(&mutex); // Lock the mutex before writing
            putchar(count);
            putchar(currentChar);
            pthread_mutex_unlock(&mutex); // Unlock the mutex after writing
            count = 1;  // Reset count for new character
        }

        currentChar = nextChar;
    }

    // Write the last character count and character
    pthread_mutex_lock(&mutex); // Lock the mutex before writing
    printf("count: %d character: %c\n",count, currentChar);
    pthread_mutex_unlock(&mutex); // Unlock the mutex after writing

    pthread_exit(NULL);
}

int main(int argc, char* args[]) {
    FILE* inputFile;

    if (argc > 2) {
        for (int file = 0; file < argc; file++) {
            printf("File %d:\n", file);
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

            // Create threads, each responsible for processing a section of the string
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
        }
    }

    return 0;
}