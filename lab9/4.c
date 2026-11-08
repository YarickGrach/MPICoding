#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank, size, i, j;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int len = rank + 1;
    int *a = new int[2 * len];
    for (i = 0; i < 2 * len; i++)
        a[i] = rank;

    int **b = new int *[2 * size];
    b[0] = new int[2 * size * size];
    for (i = 1; i < 2 * size; i++)
        b[i] = b[i - 1] + size;

    int *RC = new int[size];
    int *ds = new int[size];
    for (i = 0; i < size; i++)
    {
        RC[i] = 2 * (i + 1);
        ds[i] = 2 * i * size;
    }

    MPI_Gatherv(a, 2 * len, MPI_INT, *b, RC, ds, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("rank=%d b:\n", rank);
        for (i = 0; i < 2 * size; i++)
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
