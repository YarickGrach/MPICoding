#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int send_val = rank % 2;
    int recv_val = -1;   
    MPI_Status stat;

    int next, prev;

    if (rank % 2 == 0) {
        next = (rank + 2) % size;
        prev = (rank - 2 + size) % size;
    } else {
        next = (rank + 2) % size;
        prev = (rank - 2);
        if (prev < 1) prev = (size % 2 == 0) ? size - 1 : size - 2; 
    }

    MPI_Sendrecv(&send_val, 1, MPI_INT, next, 0,
                 &recv_val, 1, MPI_INT, prev, 0,
                 MPI_COMM_WORLD, &stat);

    printf("Process: %d recv %d from %d\n", rank, recv_val, prev);

    MPI_Finalize();
    return 0;
}
