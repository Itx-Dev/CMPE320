#include <pthread.h>

#include "request.h"
#include "io_helper.h"
#include "thread_pool.h"

queue_t jobs;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m   = PTHREAD_MUTEX_INITIALIZER;

void *worker(void *arg) {
  while (1) {
    pthread_mutex_lock(&m);
    while (Queue_Empty(&jobs)) // if empty, no job to do
      pthread_cond_wait(&fill, &m); // wait for jobs

    int fd;
    Queue_Dequeue(&jobs, &fd);
    pthread_mutex_unlock(&m);

    request_handle(fd);
    close_or_die(fd);
  }
  return NULL;
}

// n workers
int thread_pool_init(int n) {
  assert(n > 0);
  Queue_Init(&jobs);
  pthread_t t;

  for (int i = 0; i < n; i++)
    pthread_create(&t, NULL, worker, NULL);

  return 1;
}
