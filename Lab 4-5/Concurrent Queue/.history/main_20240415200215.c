#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


int val1 = 10;
int val2 = 20;
int val3 = 30;
int val4 = 40;
int val5 = 50;

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
        printf("New Head: %d\n", q->head->value);
        Queue_Dequeue(q, q->head->next->value);
    }
}

void* threadEnqueueDequeue(void *argt) {
    queue_t *queue = (queue_t *)argt;
    Queue_Enqueue(queue, val1);

    return NULL;
}


int main(int argc, char* argv[]) {
    int numThreads = 3;
    pthread_t threads[numThreads];
    queue_t *queue = malloc(sizeof(queue_t));


    Queue_Init(queue);

    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, threadEnqueueDequeue, (void*)queue);
    }

    for (int i = 0 ; i < numThreads; i++) {
        Empty_Queue(queue);
        pthread_join(threads[i], NULL);
    }
}