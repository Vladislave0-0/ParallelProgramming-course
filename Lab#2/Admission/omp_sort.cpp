#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * @brief параллельная версия qsort
 *
 * @param a указатель на массив, который нужно отсортировать
 * @param left индекс начала сортируемой части массива
 * @param right индекс конца сортируемой части массива
 * @param depth максимальная глубина рекурсии
 */
void parallel_qsort(int *a, int left, int right, int depth) {
  if (left >= right)
    return;
  int pivot = a[(left + right) / 2];
  int i = left, j = right;
  while (i <= j) {
    while (a[i] < pivot)
      i++;
    while (a[j] > pivot)
      j--;
    if (i <= j) {
      int tmp = a[i];
      a[i] = a[j];
      a[j] = tmp;
      i++;
      j--;
    }
  }
  if (depth > 0) {
#pragma omp task shared(a)
    parallel_qsort(a, left, j, depth - 1);
#pragma omp task shared(a)
    parallel_qsort(a, i, right, depth - 1);
  } else {
    parallel_qsort(a, left, j, 0);
    parallel_qsort(a, i, right, 0);
  }
}

void sort_parallel_qsort(int *a, int n) {
#pragma omp parallel
  {
#pragma omp single nowait
    parallel_qsort(a, 0, n - 1, omp_get_max_threads());
  }
}

int cmp_int(const void *pa, const void *pb) {
  int a = *(const int *)pa;
  int b = *(const int *)pb;
  return (a > b) - (a < b);
}

void sort_qsort(int *a, int n) { qsort(a, n, sizeof(int), cmp_int); }

void benchmark(FILE *file, int n, int runs) {
  if (runs == 1)
    fprintf(file, "n,threads,qsort_time,parallel_time\n");

  int *a = (int *)malloc(n * sizeof(int));
  int *b = (int *)malloc(n * sizeof(int));

  double t_q, t_p;

  for (int t = 1; t <= omp_get_max_threads(); t *= 2) {
    omp_set_num_threads(t);

    for (int i = 0; i < n; ++i)
      a[i] = rand();

    memcpy(b, a, n * sizeof(int));
    clock_t ts = clock();
    sort_qsort(b, n);
    t_q = (double)(clock() - ts) / CLOCKS_PER_SEC;

    memcpy(b, a, n * sizeof(int));
    ts = clock();
    sort_parallel_qsort(b, n);
    t_p = (double)(clock() - ts) / CLOCKS_PER_SEC;

    fprintf(file, "%d,%d,%.6f,%.6f\n", n, t, t_q, t_p);
  }

  free(a);
  free(b);
}

int main() {
  srand(0);

  int sizes[] = {1000000, 2000000, 5000000, 10000000};
  int runs = sizeof(sizes) / sizeof(sizes[0]);

  FILE *file = fopen("results.csv", "w");

  for (int i = 0; i < runs; i++)
    benchmark(file, sizes[i], i + 1);

  fclose(file);
}
