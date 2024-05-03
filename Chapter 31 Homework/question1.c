#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#include "common.h"
#include "common_threads.h"

sem_t s;

void *child(void *arg) {
  printf("child\n");
  sleep(1);
  Sem_post(&s);
  return NULL;
}

int main(int argc, char *args[]) {
  pthread_t p;
  printf("parent: begin\n");
  Sem_init(&s, 0);
  Pthread_create(&p, NULL, child, NULL);
  Sem_wait(&s);
  printf("parent: end\n");
  return 0;
}