#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
        MPI_Init(&argc, &argv);
    
        int rank, size, len;

        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
    
	char host[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(host, &len);

        printf("Processor name: %s\n", host);

        MPI_Finalize();
        return 0;
}
