#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <vector>

// Tamaño de la matriz
const int filas = 10681;
const int columnas = 7121;

// Funciones de las matrices
void crearmatriz(std::vector<std::vector<double>>& matriz) {
    #pragma omp parallel for
    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            matriz[i][j] = 1.0;  // Inicializar a 1.0 como ejemplo
        }
    }
}

int main() {
    std::vector<std::vector<double>> alpha(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> azul(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> rojo(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> verde(filas, std::vector<double>(columnas, 0.0));

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            crearmatriz(alpha);
        }
        #pragma omp section
        {
            crearmatriz(azul);
        }
        #pragma omp section
        {
            crearmatriz(rojo);
        }
        #pragma omp section
        {
            crearmatriz(verde);
        }
    }

    printf("Hola desde OpenMP\n");

    return EXIT_SUCCESS;
}