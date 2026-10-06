#include <mpi.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Количество итераций для замера (чем больше, тем точнее)
    const long long iterations = 1000000000; 
    double a = 1.000001, b = 0.000001, c;

    if (rank == 0) {
        printf("rank=%d, size=%d\n", rank, size);
        printf("Starting benchmark: %lld operations...\n", iterations);
        
        clock_t start = clock();
        
        // Цикл арифметических операций
        for (long long i = 0; i < iterations; i++) {
            c = a + b; // Одна операция сложения
        }
        
        clock_t end = clock();
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
        
        printf("Total time: %f seconds\n", time_spent);
        printf("Time per operation: %e seconds\n", time_spent / iterations);
        printf("Performance: %f MFlops\n", (iterations / time_spent) / 1000000.0);
        
        // Чтобы компилятор не выкинул цикл
        if (c == 0) printf("Ignore\n"); 
    }

    MPI_Finalize();
    return 0;
}
