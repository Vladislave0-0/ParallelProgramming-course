#include <mpi.h>

const int TOTAL_MSG =  1000;

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size < 2) {
    if (rank == 0)
      printf("This program requires at least 2 processes\n");

    MPI_Finalize();
    return 1;
  }

  int msg = 1;
  int* big_msg[1000000] = {0};
  MPI_Status status;
  double total_time = 0.0;

  for (int iter = 0; iter < TOTAL_MSG; ++iter) {
    if (rank == 0) {
      double start_time = MPI_Wtime();

      MPI_Send(&msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
      MPI_Recv(&msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);

      double end_time = MPI_Wtime();
      total_time += (end_time - start_time);
    } else if (rank == 1) {
      MPI_Recv(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Send(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
  }

  if (rank == 0) {
    double avg_time = total_time / TOTAL_MSG;

    printf("The total time for exchanging 1000 messages between processes 0 and 1: "
           " %.10f sec\n",
           total_time);

    printf("Average message exchange time between processes 0 and 1:\t\t%.10f "
           "sec\n",
           avg_time);
  }

  MPI_Finalize();
}
