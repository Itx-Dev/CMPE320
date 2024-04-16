#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

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
    if (q->head == q->tail) { // Queue is empty
        return 1;
    } else {
        return 0;
    }
}

void Empty_Queue(queue_t *q) {
    while (Queue_Empty(q) == 0) {   // While queue is not empty
        int val = q->head->value;
        if (Queue_Dequeue(q, &val) == 0) {
            printf("Dequeued value: %d\n", val);
        }
    }
    
}

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