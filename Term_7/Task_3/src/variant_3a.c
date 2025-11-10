#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define ISIZE 5000
#define JSIZE 5000

int main(int argc, char **argv) {
  int i, j;
  FILE *file;

  double **a = (double **)malloc(ISIZE * sizeof(double *));
  double **b = (double **)malloc(ISIZE * sizeof(double *));

  if (!a || !b) {
    fprintf(stderr, "Memory allocation failed for rows\n");
    return 1;
  }

  for (i = 0; i < ISIZE; ++i) {
    a[i] = (double *)malloc(JSIZE * sizeof(double));
    b[i] = (double *)malloc(JSIZE * sizeof(double));

    if (!a[i] || !b[i]) {
      fprintf(stderr, "Memory allocation failed for row %d\n", i);
      return 1;
    }
  }

  for (i = 0; i < ISIZE;
       ++i) { // Подготовительная часть - заполнение некими данными.
    for (j = 0; j < JSIZE; ++j) {
      a[i][j] = 10 * i + j;
      b[i][j] = 0;
    }
  }

  // --- Измерение времени вычислений ---
  double t_start = omp_get_wtime();

  for (i = 0; i < ISIZE; ++i) {
    for (j = 0; j < JSIZE; ++j)
      a[i][j] = sin(0.1 * a[i][j]);
  }

  for (i = 0; i < ISIZE - 1; ++i) {
    for (j = 0; j < JSIZE; ++j)
      b[i][j] = a[i + 1][j] * 1.5;
  }

  double t_end = omp_get_wtime();
  printf("Sequential time: %f seconds\n", t_end - t_start);
  // -------------------------------------

  file = fopen("./results/variant_3a.txt", "w");
  if (!file) {
    fprintf(stderr, "Error: cannot open output file\n");
    return 1;
  }

  for (i = 0; i < ISIZE; ++i) {
    for (j = 0; j < JSIZE; ++j)
      fprintf(file, "%f ", b[i][j]);
    fprintf(file, "\n");
  }
  fclose(file);

  for (i = 0; i < ISIZE; ++i) {
    free(a[i]);
    free(b[i]);
  }

  free(a);
  free(b);
}
