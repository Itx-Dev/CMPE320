#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

pthread_mutex_t lock; 

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
    if (q->head == NULL) {
        return 1;
    } else {
        return 0;
    }
}

void Empty_Queue(queue_t *q) {
    printf("%d", q->head->value);
    int newHead = 5;
    while (Queue_Empty(q) == 0) {
        Queue_Dequeue(q, &newHead);
        printf("%d", newHead);
    }
}

void* threadEnqueueDequeue(void *argt) {
    queue_t *queue = (queue_t *)argt;
    int val = queue->head->value;
    Queue_Enqueue(queue, val);
    printf("Thread Enqueued\n");
}


int main(int argc, char* argv[]) {
    int numThreads = 5;
    pthread_t threads[numThreads];
    int queue_empty = 0;
    int counter = 0;
    queue_t *queue = malloc(sizeof(queue_t));
    
    int val = 1;
    int val2 = 2;
    Queue_Init(queue);

    Queue_Enqueue(queue, val);
    Queue_Enqueue(queue, val2);
    printf("Enqueue %d\n", queue->tail->value);
    Queue_Dequeue(queue, &val2);
    Queue_Dequeue(queue, &val);
    printf("Dequeue %d\n", queue->tail->value);




    free(queue);
}