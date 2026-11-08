#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank, size, i;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int *a = new int[3];
    for (i = 0; i < 3; i++)
        a[i] = rank;

    int *b = new int[3 * size];

    MPI_Gather(a, 3, MPI_INT, b, 3, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("rank=%d b: ", rank);
        for (i = 0; i < 3 * size; i++)
            printf("%d ", b[i]);
        printf("\n");
    }

    delete[] a;
    delete[] b;
    MPI_Finalize();
    return 0;
}
