#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank, size, i;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int *a = new int[2 * size];          // буфер посылки (объявлен на всех)
    if (rank == 0)
        for (i = 0; i < 2 * size; i++)
            a[i] = i;

    int *b = new int[2];                 // буфер приёма: 2 элемента каждому
    MPI_Scatter(a, 2, MPI_INT, b, 2, MPI_INT, 0, MPI_COMM_WORLD);

    printf("rank=%d b: %d %d\n", rank, b[0], b[1]);

    delete[] a;
    delete[] b;
    MPI_Finalize();
    return 0;
}
