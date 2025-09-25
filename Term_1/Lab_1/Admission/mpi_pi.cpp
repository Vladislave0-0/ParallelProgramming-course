#include <mpi.h>
#include <cmath>

//==========================Метод Монте-Карло===========================
// Будем рандомно генерировать точки в квадрате
// со стороной 2. Успехом будем считать случай, когда точка
// попала в окружность, вписанную в такой квадрат радиуса 1.
//
// Пусть p_круг -- вероятность попасть в круг.
// Пусть p_квадрат -- вероятность попасть в квадрат
// и не попасть в круг.Тогде понятно, что:
// p_круг / p_квадрат = N_круг / N_квадрат = S_круг / S_квадрат = pi
//======================================================================
long long monteCarlo(long long num_samples) {
  long long inside_circle = 0;

  for (long long i = 0; i < num_samples; ++i) {
    double x = (double)rand() / RAND_MAX;
    double y = (double)rand() / RAND_MAX;

    if (x * x + y * y <= 1.0)
      ++inside_circle;
  }

  return inside_circle;
}

//=======Метод численного интегрирования=======
// Возьмём функцию f(x) = 4 / (1 + x^2). Тогда:
// pi = integral f(x) dx from 0 to 1
//=============================================
double integral(int steps, int rank, int size) {
  double step = 1.0 / (double)steps;
  double local_sum = 0.0;

  for (int i = rank; i < steps; i += size) {
    double x = (i + 0.5) * step;
    local_sum += 4.0 / (1.0 + x * x);
  }

  return local_sum;
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  long long total_samples = 100000000;
  int steps = 1000000;
  if (argc > 1)
    steps = atoi(argv[1]);

  MPI_Bcast(&steps, 1, MPI_INT, 0, MPI_COMM_WORLD);

  srand(time(NULL) + rank);

  // Монте-Карло
  double start_mc = MPI_Wtime();
  long long local_inside = monteCarlo(total_samples / size);
  double end_mc = MPI_Wtime();

  long long global_inside;
  MPI_Reduce(&local_inside, &global_inside, 1, MPI_LONG_LONG, MPI_SUM, 0,
             MPI_COMM_WORLD);

  // Интегральный метод
  double start_int = MPI_Wtime();
  double local_sum = integral(steps, rank, size);
  double end_int = MPI_Wtime();

  double global_sum;
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);

  if (rank == 0) {
    double pi_mc = 4.0 * global_inside / total_samples;
    double pi_int = global_sum / steps;

    printf("The tabular value of a number pi: %.15f\n\n", M_PI);
    printf("Monte-Carlo:\n\tEstimation of pi: %.15f\n", pi_mc);
    printf("\tExecution time: %f sec\n\n", end_mc - start_mc);
    printf("Numerical integration:\n\tEstimation of pi: %.15f\n", pi_int);
    printf("\tExecution time: %f sec\n", end_int - start_int);
  }

  MPI_Finalize();
}
