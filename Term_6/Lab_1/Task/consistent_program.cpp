#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Последовательная реализация численного решения уравнения переноса
// Scheme: явный левый уголок (upwind first-order)

#define T 1.0 // Время
#define X 2.0 // Длина пространственного интервала
#define a 1.0  // Коэффициент переноса
#define M 1000 // Число узлов по пространству
#define K 1000 // Число узлов по времени

double phi(double x) { return sin(M_PI * x); }

double psi(double t) { return sin(2.0 * M_PI * t / T); }

double f_func(double t, double x) { return exp(-t) * sin(M_PI * x); }

int main() {
  double tau = T / K;
  double h = X / M;
  double nu = a * tau / h;

  if (nu > 1.0) {
    fprintf(stderr,
            "Warning: Courant number nu = %g > 1. Scheme may be unstable.\n",
            nu);
  }

  // Выделение памяти для текущего и следующего слоёв
  double *u_curr = (double *)malloc((M + 1) * sizeof(double));
  double *u_next = (double *)malloc((M + 1) * sizeof(double));
  if (!u_curr || !u_next) {
    perror("malloc");
    return EXIT_FAILURE;
  }

  // Выделение памяти для хранения всех временных слоёв u_all[k][m]
  double **u_all = (double **)malloc((K + 1) * sizeof(double *));
  if (!u_all) {
    perror("malloc u_all");
    return EXIT_FAILURE;
  }
  for (int k = 0; k <= K; ++k) {
    u_all[k] = (double *)malloc((M + 1) * sizeof(double));
    if (!u_all[k]) {
      perror("malloc u_all[k]");
      return EXIT_FAILURE;
    }
  }

  // Инициализация начального слоя (k=0)
  for (int m = 0; m <= M; ++m) {
    double x = m * h;
    u_curr[m] = phi(x);
  }
  // Сохраняем k = 0
  for (int m = 0; m <= M; ++m) {
    u_all[0][m] = u_curr[m];
  }

  // Основной цикл по времени
  for (int k = 0; k < K; ++k) {
    double t = k * tau;

    // Граничное условие
    u_curr[0] = psi(t);

    // Шаг upwind-схемы
    for (int m = 1; m <= M; ++m) {
      double x = m * h;
      double fval = f_func(t, x);
      u_next[m] = u_curr[m] - nu * (u_curr[m] - u_curr[m - 1]) + tau * fval;
    }

    // Переход на следующий слой
    double *tmp = u_curr;
    u_curr = u_next;
    u_next = tmp;

    // Сохраняем слой k+1
    for (int m = 0; m <= M; ++m) {
      u_all[k + 1][m] = u_curr[m];
    }
  }

  // Сохранение всех временных слоёв
  FILE *out = fopen("./solution_output/full_solution.dat", "w");
  if (!out) {
    perror("fopen");
    return EXIT_FAILURE;
  }
  for (int k = 0; k <= K; ++k) {
    for (int m = 0; m <= M; ++m) {
      fprintf(out, "%g ", u_all[k][m]);
    }
    fprintf(out, "\n");
  }
  fclose(out);

  // Освобождение памяти
  free(u_curr);
  free(u_next);
  for (int k = 0; k <= K; ++k)
    free(u_all[k]);
  free(u_all);

  printf("Full solution saved to ./solution_output/full_solution.dat\n");
  return EXIT_SUCCESS;
}
