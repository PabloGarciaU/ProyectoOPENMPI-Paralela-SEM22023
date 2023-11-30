#include <iostream>
#include <cstdlib> // Para utilizar el comando del sistema
#include <mpi.h>

void limpiarPantalla() {
    system("clear"); // En sistemas Unix/Linux
}

void mostrarOpciones() {
    limpiarPantalla();
    std::cout << "\nMenú de opciones:\n";
    std::cout << "1) Procesar imagen\n";
    std::cout << "2) Salir\n";
}

void procesarImagen() {
    limpiarPantalla();
    std::cout << "Procesando imagen...\n";
    // Lógica para procesar la imagen
}

int main(int argc, char** argv) {
    int mi_rango, num_procesos;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mi_rango);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procesos);

    if (mi_rango == 0) {
        int opcion = 0;

        do {
            mostrarOpciones();
            std::cout << "Ingrese su opción: ";
            std::cin >> opcion;

            switch (opcion) {
                case 1:
                    procesarImagen();
                    break;
                case 2:
                    limpiarPantalla(); // Limpiar antes de salir
                    std::cout << "Saliendo del programa.\n";
                    break;
                default:
                    std::cout << "Opción no válida. Inténtelo de nuevo.\n";
            }
        } while (opcion != 2);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}