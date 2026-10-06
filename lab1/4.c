#include <mpi.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	double start = MPI_Wtime();

	double sum = 0;
	double limit = pow(10, rank + 6);

	for(int i = 0; i < limit; i++) 
		sum += (1.0 / (1 + i));

	double end = MPI_Wtime();

	printf("Limit: %e | Result: %e | Time: %e\n", limit, sum, end - start);

	MPI_Finalize();
	return 0;
}

