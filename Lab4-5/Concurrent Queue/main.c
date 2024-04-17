#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "queue.h"

int val1 = 1, val2 = 2, val3 = 3;

void* threadEnqueue(void *argt) {
    queue_t *queue = (queue_t *)argt;

    // Enqueue
    for (int i = 1; i < 4; ++i) {
        Queue_Enqueue(queue, i); // Enqueue item number i
    }

    return NULL;
}

void* threadDequeue(void *argt) {
    queue_t *queue = (queue_t *)argt;

    Empty_Queue(queue);
}



int main(int argc, char* argv[]) {
    int numThreads = 3;
    pthread_t threads[numThreads];
    queue_t *queue = malloc(sizeof(queue_t));

    Queue_Init(queue);

    for (int i = 0; i < numThreads; ++i) {
        pthread_create(&threads[i], NULL, threadEnqueue, (void*)queue);
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    Empty_Queue(queue);

    free(queue);

    return 0;
}