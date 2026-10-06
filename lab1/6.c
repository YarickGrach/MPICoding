#include <mpi.h>
#include <stdio.h>
#include <time.h> 
#include <stdlib.h>

int main(int argc, char** argv) {
        MPI_Init(&argc, &argv);
    
        int rank, size, sum = 0;

        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
    
	srand(time(NULL) + rank);

	for (int i = 0; i < 100; i++) {
		sum += rand() % 100;
        }

	printf("Rank: %3d | Avg: %3f\n", rank, sum / 100.0);

        MPI_Finalize();
        return 0;
}

