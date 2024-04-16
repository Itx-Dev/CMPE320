#include <pthread.h>
#include "queue.h"

extern queue_t jobs;
extern pthread_cond_t fill;
extern pthread_mutex_t m;

void *worker(void *arg);
int thread_pool_init(int n);