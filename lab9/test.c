#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status stat;

	int a[size];

	if (rank != 0) for (int i = 0; i < size; i++) a[i] = rank;
	int b[size * size];

	MPI_Gather (a, size, MPI_INT, b, size, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0) for (int i = size; i < size * size; i++) printf("%d ", b[i]);

	MPI_Finalize();
	return 0;
}
