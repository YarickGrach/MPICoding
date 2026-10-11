#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status stat;

	if (rank == 0) {
		int a[size][10];
		for (int i = 1; i < size; i++) {
			for (int j = 0; j < 10; j++) a[i - 1][j] = (i) * (j + 1);
			MPI_Send(a[i - 1], 10, MPI_INT, i, 777, MPI_COMM_WORLD);
			}
	}
	else {
		int b[10];
		MPI_Recv(b, 10, MPI_INT, 0, 777, MPI_COMM_WORLD, &stat);
		printf("rank: %d\n", rank);
		for (int i = 0; i < 10; i++) printf("%d ", b[i]);
		printf("\n");
	}

	MPI_Finalize();
	return 0;
}
