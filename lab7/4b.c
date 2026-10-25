#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 1000000; 
    double* my_array = new double[n];
    double* recv_array = new double[n];

    for (int i = 0; i < n; i++) {
        my_array[i] = rank * 100 + i;
        recv_array[i] = 0;
    }

    double t1, t2;
    MPI_Status status;
    int neighbor = (rank + 1) % 2;

    t1 = MPI_Wtime();

    MPI_Sendrecv(my_array, n, MPI_DOUBLE, neighbor, 0,
                 recv_array, n, MPI_DOUBLE, neighbor, 0,
                 MPI_COMM_WORLD, &status);

    t2 = MPI_Wtime();

    printf("Process %d Sendrecv time: %f\n", rank, (t2 - t1));

    delete[] my_array;
    delete[] recv_array;
    MPI_Finalize();
    return 0;
}
