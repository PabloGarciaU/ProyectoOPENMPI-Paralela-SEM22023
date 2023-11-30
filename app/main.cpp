#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int mi_rango, num_procesos;

    // Inicializa MPI
    MPI_Init(&argc, &argv);

    // Obtiene el rango del proceso actual
    MPI_Comm_rank(MPI_COMM_WORLD, &mi_rango);

    // Obtiene el número total de procesos
    MPI_Comm_size(MPI_COMM_WORLD, &num_procesos);

    // Código específico de cada proceso
    printf("Hola desde el proceso %d de %d\n", mi_rango, num_procesos);

    // Finaliza MPI
    MPI_Finalize();

    return EXIT_SUCCESS;
}