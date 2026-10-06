#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	int rank, size, n;
	MPI_Status stat;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0) {
		n = 0;
		MPI_Send(&n, 1, MPI_INT, 1, 111, MPI_COMM_WORLD);
		MPI_Recv(&n, 1, MPI_INT, size - 1, 777, MPI_COMM_WORLD, &stat);
		printf("size: %d | result: %d\n", size, n);
	}
	else {
		MPI_Recv(&n, 1, MPI_INT, rank - 1, 111, MPI_COMM_WORLD, &stat);
		n += 1;
		if (rank < size - 1) MPI_Send(&n, 1, MPI_INT, rank + 1, 111, MPI_COMM_WORLD);
		else MPI_Send(&n, 1, MPI_INT, 0, 777, MPI_COMM_WORLD);
	};

	MPI_Finalize();
	return 0;
}
