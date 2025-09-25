#include "time.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ITER 100000

long diff_nsec(struct timespec *start, struct timespec *end) {
  return (end->tv_sec - start->tv_sec) * 1000000000L +
         (end->tv_nsec - start->tv_nsec);
}

int main() {
  int fds[2];

  if (pipe(fds) < 0) {
    perror("pipe");
    exit(1);
  }

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(1);
  }

  char buf = 'A';
  struct timespec t1, t2;
  long total = 0;

  if (pid == 0) {
    for (int i = 0; i < ITER; ++i) {
      if (read(fds[0], &buf, 1) != 1)
        perror("read");
      if (write(fds[1], &buf, 1) != 1)
        perror("write");
    }
    exit(0);
  } else {
    for (int i = 0; i < ITER; ++i) {
      clock_gettime(CLOCK_MONOTONIC, &t1);

      if (write(fds[1], &buf, 1) != 1)
        perror("write");
      if (read(fds[0], &buf, 1) != 1)
        perror("read");

      clock_gettime(CLOCK_MONOTONIC, &t2);
      total += diff_nsec(&t1, &t2);
    }

    printf("Average process communication time is %ld ns\n", total / ITER);
  }
}
