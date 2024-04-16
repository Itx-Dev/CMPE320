#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct __node_t {
    int value;
    struct __node_t *next;
} node_t;

typedef struct __queue_t {
    node_t *head;
    node_t *tail;
    pthread_mutex_t head_lock, tail_lock;
} queue_t;

void Queue_Init(queue_t *q) {
    node_t *tmp = malloc(sizeof(node_t));
    tmp->next = NULL;
    q->head = q->tail = tmp;
    pthread_mutex_init(&q->head_lock, NULL);
    pthread_mutex_init(&q->tail_lock, NULL);
}

void Queue_Enqueue(queue_t *q, int value) {
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = value;
    tmp->next = NULL;

    pthread_mutex_lock(&q->tail_lock);
    q->tail->next = tmp;
    q->tail = tmp;
    pthread_mutex_unlock(&q->tail_lock);
}

int Queue_Dequeue(queue_t *q, int *value) {
    pthread_mutex_lock(&q->head_lock);
    node_t *tmp = q->head;
    node_t *new_head = tmp->next;
    if (new_head == NULL) {
        pthread_mutex_unlock(&q->head_lock);
        return -1; // queue was empty
    }
    *value = new_head->value;
    q->head = new_head;
    pthread_mutex_unlock(&q->head_lock);
    free(tmp);
    return 0;
}


int Queue_Empty(queue_t *q) {
    if (q->tail == q->head) { // Queue is empty
        return 1;
    } else {
        return 0;
    }
}

void Empty_Queue(queue_t *q) {
    while (Queue_Empty(q) == 0) {   // While queue is full
        Queue_Dequeue(q, &(q->head->value));  // Dequeue next value
        printf("New Head: %d\n", q->head->value); 
    }
}

void* threadEnqueueDequeue1(void *argt) {
    queue_t *queue = (queue_t *)argt;
    int val1 = 1, val2 = 2, val3 = 3;
    Queue_Enqueue(queue, val1);
    Queue_Enqueue(queue, val2);
    Queue_Enqueue(queue, val3);

    return NULL;
}

void* threadEnqueueDequeue2(void *argt) {
    queue_t *queue = (queue_t *)argt;
    int val1 = 4, val2 = 5, val3 = 6;
    Queue_Enqueue(queue, val1);
    Queue_Enqueue(queue, val2);
    Queue_Enqueue(queue, val3);

    return NULL;
}

void* threadEnqueueDequeue3(void *argt) {
    queue_t *queue = (queue_t *)argt;
    int val1 = 7, val2 = 8, val3 = 9;

    sleep(5);
    Queue_Enqueue(queue, val1);
    Queue_Enqueue(queue, val2);
    Queue_Enqueue(queue, val3);

    return NULL;
}


int main(int argc, char* argv[]) {
    int numThreads = 3;
    pthread_t *threads = malloc(numThreads * sizeof(pthread_t));
    queue_t *queue = malloc(sizeof(queue_t));


    Queue_Init(queue);

    // Array of thread functions
    void *(*threadFunctions[])(void*) = {threadEnqueueDequeue1, threadEnqueueDequeue2, threadEnqueueDequeue3};

    // Create threads
    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, threadFunctions[i], (void*)queue);
    }

    // Join threads
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }



    Empty_Queue(queue);
}