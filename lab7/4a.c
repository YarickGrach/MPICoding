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

    t1 = MPI_Wtime();

    if (rank == 0) {
        MPI_Send(my_array, n, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(recv_array, n, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);
    } else {
        MPI_Recv(recv_array, n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Send(my_array, n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    t2 = MPI_Wtime();

    printf("Process %d Send/Recv time: %f\n", rank, (t2 - t1));

    delete[] my_array;
    delete[] recv_array;
    MPI_Finalize();
    return 0;
}
