#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define ISIZE 5000
#define JSIZE 5000

int main() {
  int i, j;
  FILE *file;

  double **a = (double **)malloc(ISIZE * sizeof(double *));
  if (!a) {
    fprintf(stderr, "Memory allocation failed for rows\n");
    return 1;
  }
  for (i = 0; i < ISIZE; ++i) {
    a[i] = (double *)malloc(JSIZE * sizeof(double));
    if (!a[i]) {
      fprintf(stderr, "Memory allocation failed for row %d\n", i);
      return 1;
    }
  }

  // Подготовительная часть - заполнение некими данными.
  for (i = 0; i < ISIZE; ++i) {
    for (j = 0; j < JSIZE; ++j)
      a[i][j] = 10.0 * i + j;
  }

  // --- Измерение времени вычислений ---
  double t_start = omp_get_wtime();

  for (i = 0; i < ISIZE; ++i) {
    for (j = 0; j < JSIZE; ++j)
      a[i][j] = sin(2.0 * a[i][j]);
  }

  double t_end = omp_get_wtime();
  printf("Sequential time: %f seconds\n", t_end - t_start);
  // -------------------------------------

  file = fopen("./results/reference.txt", "w");
  if (!file) {
    fprintf(stderr, "Error: cannot open output file\n");
    return 1;
  }

  for (i = 0; i < ISIZE; ++i) {
    for (j = 0; j < JSIZE; ++j)
      fprintf(file, "%f ", a[i][j]);
    fprintf(file, "\n");
  }

  fclose(file);

  for (i = 0; i < ISIZE; ++i)
    free(a[i]);
  free(a);
}
