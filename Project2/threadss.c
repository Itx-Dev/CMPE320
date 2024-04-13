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
    struct arg_val_t {
        char* rawString;
        int start;
        int length;
    } arg_val; 
    struct ret_val_t {
        char firstChar;
        char lastChar;
        int firstCharCount;
        int lastCharCount;
    } ret_val;
} arg_t;

void* threadFunction(void* arg) {
    arg_t* argStructs = (arg_t*)arg; // Take in parameter and result structures

    char* rawString = argStructs->arg_val.rawString;
    int start = argStructs->arg_val.start;
    int length = argStructs->arg_val.length;

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
            pthread_mutex_unlock(&mutex); // Unlock the mutex after writing
            count = 1;  // Reset count for new character
        }
        currentChar = nextChar;
    }
    argStructs->ret_val.lastChar = currentChar;
    argStructs->ret_val.lastCharCount = count;
    pthread_exit(NULL);
}

int main(int argc, char* args[]) {
    FILE* inputFile;

    if (argc > 2) {
        for (int file = 1; file < argc; file++) {
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
                arg_t threadParameter[numOfThreads];
                
                // Create threads, each responsible for processing a section of the string
                for (int threadNum = 0; threadNum < numOfThreads; threadNum++) {
                    threadParameter[threadNum].arg_val.rawString = string;
                    threadParameter[threadNum].arg_val.start = threadNum * sectionLength;
                    threadParameter[threadNum].arg_val.length = (threadNum == numOfThreads - 1) ? stringLength - threadParameter[threadNum].arg_val.start : sectionLength;
                    threadParameter[threadNum].ret_val.lastCharCount = 0;
                    pthread_create(&threads[threadNum], NULL, threadFunction, (void*)&threadParameter[threadNum]);
                }

                // Wait for all threads to complete
                for (int waitingThread = 0; waitingThread < numOfThreads; waitingThread++) {
                    pthread_join(threads[waitingThread], NULL);
                    printf("Last Character: %c has %d occurances\n", threadParameter[waitingThread].ret_val.lastChar, threadParameter[waitingThread].ret_val.lastCharCount);
                }
        }
    }

    return 0;
}
