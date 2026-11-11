#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank, size, i;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int total = size * (size + 1);
    int *a = new int[total];
    if (rank == 0)
        for (i = 0; i < total; i++)
            a[i] = i;

    int len = 2 * (rank + 1);            // длина блока процесса rank
    int *b = new int[len];               // буфер приёма

    int *RC = new int[size];             // число элементов каждому процессу
    int *ds = new int[size];             // смещения
    for (i = 0; i < size; i++)
    {
        RC[i] = 2 * (i + 1);
        ds[i] = i * (i + 1);             // сумма 2+4+...+2i
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
