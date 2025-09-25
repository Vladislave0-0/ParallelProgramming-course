#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITER 100000

long diff_nsec(struct timespec *start, struct timespec *end) {
  return (end->tv_sec - start->tv_sec) * 1000000000L +
         (end->tv_nsec - start->tv_nsec);
}

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int flag = 0;
struct timespec t1, t2;
long total = 0;

void *worker(void *arg) {
  for (int i = 0; i < ITER; ++i) {
    pthread_mutex_lock(&mtx);

    while (flag == 0)
      pthread_cond_wait(&cond, &mtx);

    clock_gettime(CLOCK_MONOTONIC, &t2);

    total += diff_nsec(&t1, &t2);
    flag = 0;

    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mtx);
  }

  return nullptr;
}

int main() {
  pthread_t th;
  pthread_create(&th, NULL, worker, NULL);

  for (int i = 0; i < ITER; ++i) {
    pthread_mutex_lock(&mtx);
    flag = 1;

    clock_gettime(CLOCK_MONOTONIC, &t1);
    pthread_cond_signal(&cond);

    while (flag == 1)
      pthread_cond_wait(&cond, &mtx);

    pthread_mutex_unlock(&mtx);
  }

  pthread_join(th, NULL);
  printf("Average thread communication time is %ld ns\n", total / ITER);
}
