#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "queue.h"

void* threadEnqueue1(void *argt) {
    queue_t *queue = (queue_t *)argt;
    int val1 = 1, val2 = 2, val3 = 3;
    Queue_Enqueue(queue, val1);
    sleep(1);
    Queue_Enqueue(queue, val2);
    sleep(2);
    Queue_Enqueue(queue, val3);
    return NULL;
}

void* threadEnqueue2(void *argt) {
    queue_t *queue = (queue_t *)argt;
    int val1 = 4, val2 = 5, val3 = 6;
    Queue_Enqueue(queue, val1);
    sleep(1);
    Queue_Enqueue(queue, val2);
    sleep(1);
    Queue_Enqueue(queue, val3);
    return NULL;
}

void* threadEnqueue3(void *argt) {
    queue_t *queue = (queue_t *)argt;
    int val1 = 7, val2 = 8, val3 = 9;
    Queue_Enqueue(queue, val1);
    sleep(4);
    Queue_Enqueue(queue, val2);
    sleep(2);
    Queue_Enqueue(queue, val3);
    return NULL;
}


int main(int argc, char* argv[]) {
    int numThreads = 3;
    pthread_t thread1, thread2, thread3;
    queue_t *queue = malloc(sizeof(queue_t));

    Queue_Init(queue);

    pthread_create(&thread1, NULL, threadEnqueue1, (void*)queue);
    pthread_create(&thread2, NULL, threadEnqueue2, (void*)queue);
    pthread_create(&thread3, NULL, threadEnqueue3, (void*)queue);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    Empty_Queue(queue);

    free(queue);

    return 0;
}