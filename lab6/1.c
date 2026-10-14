#include <mpi.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0){
		int matr[size - 1][size - 1];
		for (int i = 1; i < size; i++) {
			MPI_Status stat;
			int count, source;

			MPI_Probe(MPI_ANY_SOURCE, 777, MPI_COMM_WORLD, &stat);

			source = stat.MPI_SOURCE;

			MPI_Recv(matr[source], count, MPI_INT, source, 777, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		for (int i = 0; i < size; i++) {
			for (int j = 0; j < i; j++) printf("%d ", matr[i][j]);
			printf("\n");
		}
	}

	else {
		srand(time(NULL) + rank);
		int count = rank;
		int data[size];

		for (int i = 0; i < count; i++) data[i] = rand() % 100 + 1;
		printf("rank: %d\narr: %d", rank, data[0]);
		for (int i = 1; i < count; i++) printf(", %d", data[i]);
		printf("\n");

		MPI_Send(data, count, MPI_INT, 0, 777, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}
