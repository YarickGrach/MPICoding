#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
        MPI_Init(&argc, &argv);
    
        int rank, size, len = 10, sum = 0;

        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    	int a[len];
	for (int i = 0; i < len; i++) a[i] = i;
	for (int k = 0; k <= rank; k++) sum += k;

	printf("Rank: %d | Result: %d\n", rank, sum);

        MPI_Finalize();
        return 0;
}

