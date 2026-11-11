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
            n = 4;                       // значение по умолчанию для теста
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // a[2*size][n] — на 0-м процессе
    int **a = new int *[2 * size];
    a[0] = new int[2 * size * n];
    for (i = 1; i < 2 * size; i++)
        a[i] = a[i - 1] + n;
    if (rank == 0)
        for (i = 0; i < 2 * size; i++)
            for (j = 0; j < n; j++)
                a[i][j] = i;

    // b[2][n] — на каждом процессе
    int **b = new int *[2];
    b[0] = new int[2 * n];
    b[1] = b[0] + n;

    MPI_Scatter(*a, 2 * n, MPI_INT, *b, 2 * n, MPI_INT, 0, MPI_COMM_WORLD);

    printf("rank=%d b:\n", rank);
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < n; j++)
            printf("%d ", b[i][j]);
        printf("\n");
    }

    delete[] a[0];  delete[] a;
    delete[] b[0];  delete[] b;
    MPI_Finalize();
    return 0;
}
