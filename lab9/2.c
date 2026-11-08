#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank, size, i;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = size - rank;
    int *a = new int[n];
    for (i = 0; i < n; i++)
        a[i] = rank;

    int total = size * (size + 1) / 2;
    int *b = new int[total];

    int *RC = new int[size];
    int *ds = new int[size];
    RC[0] = size;  ds[0] = 0;
    for (i = 1; i < size; i++)
    {
        RC[i] = size - i;
        ds[i] = ds[i - 1] + RC[i - 1];
    }

    MPI_Gatherv(a, n, MPI_INT, b, RC, ds, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("rank=%d b: ", rank);
        for (i = 0; i < total; i++)
            printf("%d ", b[i]);
        printf("\n");
    }

    delete[] a;
    delete[] b;
    delete[] RC;
    delete[] ds;
    MPI_Finalize();
    return 0;
}
