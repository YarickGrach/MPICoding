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
            n = 4;                 // значение по умолчанию, если ввода нет
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int **a = new int *[2];
    a[0] = new int[2 * n];
    a[1] = a[0] + n;
    for (i = 0; i < 2; i++)
        for (j = 0; j < n; j++)
            a[i][j] = rank;

    int **b = new int *[2 * size];
    b[0] = new int[2 * size * n];
    for (i = 1; i < 2 * size; i++)
        b[i] = b[i - 1] + n;

    MPI_Gather(*a, 2 * n, MPI_INT, *b, 2 * n, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("rank=%d b:\n", rank);
        for (i = 0; i < 2 * size; i++)
        {
            for (j = 0; j < n; j++)
                printf("%d ", b[i][j]);
            printf("\n");
        }
    }

    delete[] a[0];  delete[] a;
    delete[] b[0];  delete[] b;
    MPI_Finalize();
    return 0;
}
