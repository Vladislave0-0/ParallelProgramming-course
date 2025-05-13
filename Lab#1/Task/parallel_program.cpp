#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define T 1.0
#define X 2.0
#define a 1.0
#define M 1000
#define K 1000

// Функции условий и правой части
static inline double phi(double x) { 
  return sin(M_PI * x); 
}
static inline double psi(double t) { 
  return sin(2.0 * M_PI * t / T); 
}
static inline double f_func(double t, double x) { 
  return exp(-t) * sin(M_PI * x); 
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  double h = X / M;
  double tau = T / K;
  double nu = a * tau / h;
  if (rank == 0 && nu > 1.0) {
    fprintf(stderr, "Предупреждение: число nu = %g > 1. Схема нестабильна.\n", nu);
  }

  int base = M / size;
  int rem = M % size;
  int start = rank * base + (rank < rem ? rank : rem);
  int local = base + (rank < rem ? 1 : 0);

  double *u_curr = (double *)malloc((local + 2) * sizeof(double));
  double *u_next = (double *)malloc((local + 2) * sizeof(double));
  if (!u_curr || !u_next) {
    perror("malloc");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  for (int i = 0; i < local + 2; ++i) {
    int gm = start + i - 1;
    u_curr[i] = (gm >= 0 && gm <= M) ? phi(gm * h) : 0.0;
  }

  MPI_Barrier(MPI_COMM_WORLD);
  double t0 = MPI_Wtime();

  for (int k = 0; k < K; ++k) {
    double t = k * tau;
    if (rank == 0)
      u_curr[1] = psi(t);

    double send_right = u_curr[local];
    double send_left = u_curr[1];
    double recv_left, recv_right;
    MPI_Status st;
    
    // обмен левой границей
    if (rank > 0) {
      MPI_Sendrecv(&send_left, 1, MPI_DOUBLE, rank - 1, 0, &recv_left, 1,
                   MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &st);
      u_curr[0] = recv_left;
    } else {
      u_curr[0] = psi(t);
    }
    // обмен правой границей
    if (rank < size - 1) {
      MPI_Sendrecv(&send_right, 1, MPI_DOUBLE, rank + 1, 0, &recv_right, 1,
                   MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &st);
      u_curr[local + 1] = recv_right;
    } else {
      u_curr[local + 1] = u_curr[local];
    }

    for (int i = 1; i <= local; ++i) {
      double x = (start + i - 1) * h;
      u_next[i] =
          u_curr[i] - nu * (u_curr[i] - u_curr[i - 1]) + tau * f_func(t, x);
    }
    double *tmp = u_curr;
    u_curr = u_next;
    u_next = tmp;
  }

  double t1 = MPI_Wtime();
  double local_time = t1 - t0;
  double max_time;
  MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if (rank == 0)
    printf("Time with %d procs: %f s\n", size, max_time);

  // Сбор итогового слоя
  double *global = NULL;
  int *counts = NULL, *displs = NULL;
  if (rank == 0) {
    global = (double *)malloc((M + 1) * sizeof(double));
    counts = (int *)malloc(size * sizeof(int));
    displs = (int *)malloc(size * sizeof(int));
  }
  int sendcount = local;
  MPI_Gather(&sendcount, 1, MPI_INT, counts, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    displs[0] = 0;
    for (int i = 1; i < size; ++i)
      displs[i] = displs[i - 1] + counts[i - 1];
  }
  MPI_Gatherv(&u_curr[1], sendcount, MPI_DOUBLE, global, counts, displs,
              MPI_DOUBLE, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    FILE *f = fopen("./solution_output/parallel_solution.dat", "w");
    for (int i = 0; i <= M; ++i)
      fprintf(f, "%g %g\n", i * h, global[i]);
    fclose(f);
    free(global);
    free(counts);
    free(displs);
  }

  free(u_curr);
  free(u_next);
  MPI_Finalize();
}
