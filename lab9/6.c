#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank, size, i, j;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int len = size - rank;
    int *a = new int[len];
    for (i = 0; i < len; i++)
        a[i] = rank;

    int **b = new int *[size];
    b[0] = new int[size * size];
    for (i = 1; i < size; i++)
        b[i] = b[i - 1] + size;
    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
            b[i][j] = 0;

    int *RC = new int[size];
    int *ds = new int[size];
    for (i = 0; i < size; i++)
    {
        RC[i] = size - i;
        ds[i] = i * size + i;
    }

    MPI_Gatherv(a, len, MPI_INT, *b, RC, ds, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("rank=%d b:\n", rank);
        for (i = 0; i < size; i++)
        {
            for (j = 0; j < size; j++)
                printf("%d ", b[i][j]);
            printf("\n");
        }
    }

    delete[] a;
    delete[] b[0];  delete[] b;
    delete[] RC;
    delete[] ds;
    MPI_Finalize();
    return 0;
}
