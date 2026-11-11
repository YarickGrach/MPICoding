#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank, size, i, j;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // a[size][size] — на всех процессах
    int **a = new int *[size];
    a[0] = new int[size * size];
    for (i = 1; i < size; i++)
        a[i] = a[i - 1] + size;
    if (rank == 0)
        for (i = 0; i < size; i++)
            for (j = 0; j < size; j++)
                a[i][j] = i;

    int len = size - rank;               // длина блока процесса rank
    int *b = new int[len];

    int *RC = new int[size];
    int *ds = new int[size];
    for (i = 0; i < size; i++)
    {
        RC[i] = size - i;                // первые (size-i) элементов строки i
        ds[i] = i * size;                // начало строки i
    }

    MPI_Scatterv(*a, RC, ds, MPI_INT, b, len, MPI_INT, 0, MPI_COMM_WORLD);

    printf("rank=%d b: ", rank);
    for (i = 0; i < len; i++)
        printf("%d ", b[i]);
    printf("\n");

    delete[] a[0];  delete[] a;
    delete[] b;
    delete[] RC;
    delete[] ds;
    MPI_Finalize();
    return 0;
}
