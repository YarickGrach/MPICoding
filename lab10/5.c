#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank, size, i, j;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int total = 0;
    for (i = 0; i < size; i++)
        total += 2 * (i + 1);            // суммарная длина всех блоков

    int *a = new int[total];             // буфер посылки (на всех)
    if (rank == 0)
    {
        int k = 0;
        for (i = 0; i < size; i++)
            for (j = 0; j < 2 * (i + 1); j++)
                a[k++] = i;              // блок i: 2*(i+1) элементов, все = i
    }

    int len = 2 * (rank + 1);            // длина блока процесса rank
    int *b = new int[len];

    int *RC = new int[size];
    int *ds = new int[size];
    int off = 0;
    for (i = 0; i < size; i++)
    {
        RC[i] = 2 * (i + 1);
        ds[i] = off;
        off  += RC[i];
    }

    MPI_Scatterv(a, RC, ds, MPI_INT, b, len, MPI_INT, 0, MPI_COMM_WORLD);

    printf("rank=%d b: ", rank);
    for (i = 0; i < len; i++)
        printf("%d ", b[i]);
    printf("\n");

    delete[] a;
    delete[] b;
    delete[] RC;
    delete[] ds;
    MPI_Finalize();
    return 0;
}
