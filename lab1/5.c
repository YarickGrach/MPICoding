#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank, size, n;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank % 2 == 0) n = 2 * rank;
    else n = 3 * rank;

    printf("Rank: %2d | n: %2d\n", rank, n);
    
    MPI_Finalize();
    return 0;
}

