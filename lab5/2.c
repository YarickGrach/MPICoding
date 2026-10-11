#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status stat;

	if (rank != 0) {
		int a[10];
		for (int i = 1; i <= 10; i++) a[i - 1] = rank * i;
		MPI_Send(a, 10, MPI_INT, 0, 777, MPI_COMM_WORLD);
	}
	else {
		int b[size - 1][10];
		for (int i = 1; i < size; i++) MPI_Recv(b[i - 1], 10, MPI_INT, i, 777, MPI_COMM_WORLD, &stat);
		for (int i = 0; i < size - 1; i++) {
			for (int j = 0; j < 10; j++) printf("%d ", b[i][j]);
			printf("\n");
		}
	}

	MPI_Finalize();
	return 0;
}
