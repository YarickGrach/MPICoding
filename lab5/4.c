#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status stat;

	if (rank != 0) {
		int a[3][10];
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 10; j++) a[i][j] = (i + 1) * (j + 1) * rank;
		MPI_Send(a, 30, MPI_INT, 0, 777, MPI_COMM_WORLD);
	}
	else {
		int b[(size - 1) * 3][10];
		for (int i = 1; i < size; i++) {
			MPI_Recv(&b[3 * (i - 1)][0], 30, MPI_INT, i, 777, MPI_COMM_WORLD, &stat);
		}
		for (int i = 0; i < (size - 1) * 3; i++){
			for (int j = 0; j < 10; j++) printf("%d ", b[i][j]);
			printf("\n");
		}
	}

	MPI_Finalize();
	return 0;
}
