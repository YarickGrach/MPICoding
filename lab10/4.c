#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank, size, n = 0, i, j;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        if (scanf("%d", &n) != 1)
            n = 5;                       // значение по умолчанию для теста
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // a[n][n] на всех процессах (на 0-м заполняется)
    int **a = new int *[n];
    a[0] = new int[n * n];
    for (i = 1; i < n; i++)
        a[i] = a[i - 1] + n;
    if (rank == 0)
        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                a[i][j] = 10 * (i + 1) + (j + 1);

    // оптимальное распределение строк
    int base = n / size;
    int rem  = n % size;

    int *rows = new int[size];           // число строк каждому процессу
    int *RC   = new int[size];           // число элементов (строк*n)
    int *ds   = new int[size];           // смещения (в элементах)
    int off = 0;
    for (i = 0; i < size; i++)
    {
        rows[i] = base + (i < rem ? 1 : 0);
        RC[i]   = rows[i] * n;
        ds[i]   = off;
        off    += RC[i];
    }

    // приёмный буфер: rows[rank] строк по n
    int myrows = rows[rank];
    int **b = new int *[myrows];
    b[0] = new int[myrows * n];
    for (i = 1; i < myrows; i++)
        b[i] = b[i - 1] + n;

    MPI_Scatterv(*a, RC, ds, MPI_INT, *b, RC[rank], MPI_INT, 0, MPI_COMM_WORLD);

    printf("rank=%d (%d rows):\n", rank, myrows);
    for (i = 0; i < myrows; i++)
    {
        for (j = 0; j < n; j++)
            printf("%d ", b[i][j]);
        printf("\n");
    }

    delete[] a[0];  delete[] a;
    delete[] b[0];  delete[] b;
    delete[] rows;  delete[] RC;  delete[] ds;
    MPI_Finalize();
    return 0;
}
