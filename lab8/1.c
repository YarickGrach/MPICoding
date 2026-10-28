#include <mpi.h>
#include <stdio.h>

void My_Bcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
	int rank, size;
    	MPI_Comm_rank(comm, &rank);
    	MPI_Comm_size(comm, &size);

    	MPI_Status stat;

    	int rel_rank = (rank - root + size) % size;

    	for (int mask = 1; mask < size; mask *= 2) {
        	if (rel_rank < mask) {
            		int dest_rel = rel_rank + mask;
            		if (dest_rel < size) {
                		int dest = (root + dest_rel) % size;
               			MPI_Send(buffer, count, datatype, dest, 0, comm);
            		}

        	} else if (rel_rank < 2 * mask) {
            		int source_rel = rel_rank - mask;
            		int source = (root + source_rel) % size;
            		MPI_Recv(buffer, count, datatype, source, 0, comm, &stat);
        	  }
    	}
}


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    static int a[1000000];

    if (rank == 0) for (int i = 0; i < 1000000; i++) a[i] = i;

    double t0 = MPI_Wtime();
    My_Bcast(a, 1000000, MPI_INT, 0, MPI_COMM_WORLD);
    double t1 = MPI_Wtime();

    if (rank == 0) for (int i = 0; i < 1000000; i++) a[i] = i * 2;

    double t2 = MPI_Wtime();
    MPI_Bcast(a, 1000000, MPI_INT, 0, MPI_COMM_WORLD);
    double t3 = MPI_Wtime();

    if (rank == 0) {
        printf("rank %d, first = %d, last = %d, My_Bcast() time = %f, MPI_Bcast() time = %f\n",
               rank, a[0], a[999999], (t1 - t0), (t3 - t2));
    }

    MPI_Finalize();
    return 0;
}
