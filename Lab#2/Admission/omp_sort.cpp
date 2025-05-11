#include <omp.h>
#include <stdlib.h>

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

int main() {}
