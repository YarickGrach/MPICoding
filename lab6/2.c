#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    long long N = 1000; // Значение по умолчанию (10^3)
    double start_time, end_time, local_sum, total_sum, temp_sum;
    long long i;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc > 1) {
        N = atoll(argv[1]);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    long long items_per_proc = N / size;
    long long remainder = N % size;
    
    long long my_start = rank * items_per_proc + 1;
    long long my_end = (rank + 1) * items_per_proc;
    
    if (rank < remainder) {
        my_start += rank;
        my_end += rank + 1;
    } else {
        my_start += remainder;
        my_end += remainder;
    }

    local_sum = 0.0;
    if (my_start <= my_end) {
        for (i = my_start; i <= my_end; i++) {
            local_sum += 1.0 / (i + 1.0);
        }
    }

    if (rank == 0) {
        total_sum = local_sum;
        
        for (int i = 1; i < size; i++) {
            MPI_Status status;
            int source;
            
            // --- ВАРИАНТ 1: Без MPI_ANY_SOURCE (строго по порядку) ---
            //source = i;
            //MPI_Recv(&temp_sum, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // --- ВАРИАНТ 2: С MPI_ANY_SOURCE ---
             MPI_Recv(&temp_sum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
             source = status.MPI_SOURCE;
            
            total_sum += temp_sum;
        }
        
        end_time = MPI_Wtime();
        printf("N = %lld, Processes = %d, Time = %f sec, Result = %.10f\n", 
               N, size, end_time - start_time, total_sum);
               
    } else {
        MPI_Send(&local_sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
