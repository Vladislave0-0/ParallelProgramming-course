#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define ISIZE 5000
#define JSIZE 5000

int main(int argc, char **argv) {
  int i, j;
  FILE *file;
  int rank, size;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Вычислим количество строк для каждого процесса.
  int rows_per_proc = ISIZE / size;
  int remainder = ISIZE % size;

  // Определим начальную и конечную строку для текущего процесса.
  int start_row, end_row;
  if (rank < remainder) {
    start_row = rank * (rows_per_proc + 1);
    end_row = start_row + rows_per_proc + 1;
  } else {
    start_row =
        remainder * (rows_per_proc + 1) + (rank - remainder) * rows_per_proc;
    end_row = start_row + rows_per_proc;
  }

  // Выделяем память для локальных частей массивов
  int local_rows = end_row - start_row;
  double **local_a = (double **)malloc(local_rows * sizeof(double *));
  double **local_b = (double **)malloc(local_rows * sizeof(double *));

  if (!local_a || !local_b) {
    fprintf(stderr, "Memory allocation failed for rows\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  for (i = 0; i < local_rows; ++i) {
    local_a[i] = (double *)malloc(JSIZE * sizeof(double));
    local_b[i] = (double *)malloc(JSIZE * sizeof(double));
    if (!local_a[i] || !local_b[i]) {
      fprintf(stderr, "Memory allocation failed for row %d\n", i);
      MPI_Abort(MPI_COMM_WORLD, 1);
    }
  }

  // Подготовительная часть - заполнение некими данными.
  for (i = 0; i < local_rows; ++i) {
    for (j = 0; j < JSIZE; ++j) {
      int global_i = start_row + i;
      local_a[i][j] = 10 * global_i + j;
      local_b[i][j] = 0;
    }
  }

  // Синхронизация перед началом вычислений.
  MPI_Barrier(MPI_COMM_WORLD);

  // --- Измерение времени вычислений ---
  double t_start = MPI_Wtime();

  // Первый цикл полностью параллельный.
  for (i = 0; i < local_rows; ++i) {
    for (j = 0; j < JSIZE; ++j)
      local_a[i][j] = sin(0.1 * local_a[i][j]);
  }

  // Второй цикл требует обмена граничными строками. Поэтому выделяем буферы
  // для обмена.
  double *send_buffer = NULL;
  double *recv_buffer = NULL;

  if (local_rows > 0) {
    send_buffer = (double *)malloc(JSIZE * sizeof(double));
    recv_buffer = (double *)malloc(JSIZE * sizeof(double));
  }

  for (i = 0; i < local_rows - 1; ++i) {
    for (j = 0; j < JSIZE; ++j)
      local_b[i][j] = local_a[i + 1][j] * 1.5;
  }

  // Обработка границы между процессами.
  if (rank < size - 1 && local_rows > 0) {
    for (j = 0; j < JSIZE; ++j)
      send_buffer[j] = local_a[local_rows - 1][j];

    MPI_Send(send_buffer, JSIZE, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
  }

  if (rank > 0 && local_rows > 0) {
    MPI_Recv(recv_buffer, JSIZE, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD,
             &status);

    int last_local_row = local_rows - 1;
    if (start_row + last_local_row < ISIZE - 1) {
      for (j = 0; j < JSIZE; ++j)
        local_b[last_local_row][j] = recv_buffer[j] * 1.5;
    }
  }

  double t_end = MPI_Wtime();
  if (rank == 0) {
    printf("Parallel time: %f seconds\n", t_end - t_start);
  }
  // -------------------------------------

  // Сбор результатов в процессе 0.
  if (rank == 0) {
    double **full_b = (double **)malloc(ISIZE * sizeof(double *));
    for (i = 0; i < ISIZE; ++i) {
      full_b[i] = (double *)malloc(JSIZE * sizeof(double));
    }

    for (i = 0; i < local_rows && (start_row + i) < ISIZE; ++i) {
      for (j = 0; j < JSIZE; ++j) {
        full_b[start_row + i][j] = local_b[i][j];
      }
    }

    // Получаем данные от других процессов.
    for (int proc = 1; proc < size; ++proc) {
      int proc_start, proc_end, proc_rows;

      if (proc < remainder) {
        proc_start = proc * (rows_per_proc + 1);
        proc_end = proc_start + rows_per_proc + 1;
      } else {
        proc_start = remainder * (rows_per_proc + 1) +
                     (proc - remainder) * rows_per_proc;
        proc_end = proc_start + rows_per_proc;
      }
      proc_rows = proc_end - proc_start;

      for (i = 0; i < proc_rows && (proc_start + i) < ISIZE; ++i) {
        MPI_Recv(full_b[proc_start + i], JSIZE, MPI_DOUBLE, proc, 0,
                 MPI_COMM_WORLD, &status);
      }
    }

    file = fopen("./results/variant_3a.mpi.txt", "w");
    if (!file) {
      fprintf(stderr, "Error: cannot open output file\n");
      MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (i = 0; i < ISIZE; ++i) {
      for (j = 0; j < JSIZE; ++j)
        fprintf(file, "%f ", full_b[i][j]);
      fprintf(file, "\n");
    }
    fclose(file);

    for (i = 0; i < ISIZE; ++i)
      free(full_b[i]);
    free(full_b);

  } else {
    // Отправка данных процессу 0.
    for (i = 0; i < local_rows && (start_row + i) < ISIZE; ++i)
      MPI_Send(local_b[i], JSIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }

  for (i = 0; i < local_rows; ++i) {
    free(local_a[i]);
    free(local_b[i]);
  }
  free(local_a);
  free(local_b);

  if (send_buffer)
    free(send_buffer);
  if (recv_buffer)
    free(recv_buffer);

  MPI_Finalize();
}