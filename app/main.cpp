#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
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
            matriz[i][j] = 0;  // Inicializar a 0 como ejemplo
        }
    }
}

void llenarmatriz(std::vector<std::vector<double>>& matriz) {
    #pragma omp parallel for
    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            // Llenar la matriz con cero
            matriz[i][j] = 0;
        }
    }
}

void corregirmatriz(){

}

void unirmatriz(){

}

void generarimagen(){

}

// Función para llenar la matriz desde un archivo de texto
void leerarchivo(const std::string& nombreArchivo, std::vector<std::vector<double>>& matriz) {
    std::ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo: " << nombreArchivo << std::endl;
        return;
    }

    // Reinicializa la matriz con ceros antes de llenarla con nuevos valores
    matriz.assign(filas, std::vector<double>(columnas, 0.0));

    // Lee valores del archivo y llena la matriz
    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            archivo >> matriz[i][j];
        }
    }

    archivo.close();
}

void saludo() {
    int thread_id = omp_get_thread_num();
    printf("Hola desde OpenMP en el hilo %d\n", thread_id);
}

void valorinicial(const std::vector<std::vector<double>>& matriz) {
    int thread_id = omp_get_thread_num();

    printf("Valores iniciales en el hilo %d: ", thread_id);

    // Imprime los primeros 5 valores de la matriz asignada al hilo
    for (int i = 0; i < 5 && i < filas; ++i) {
        for (int j = 0; j < 5 && j < columnas; ++j) {
            printf("%.2f ", matriz[i][j]);
        }
    }
    printf("\n");
}

int main() {
    std::vector<std::vector<double>> alfa(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> azul(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> rojo(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> verde(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> resultado(filas, std::vector<double>(columnas, 0.0));

    // Se establece el número de hilos para OpenMP
    omp_set_num_threads(4);

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            crearmatriz(alfa);
            llenarmatriz(alfa);
            leerarchivo("alfa.txt", alfa);
            //saludo();
            //valorinicial(alfa);
        }
        #pragma omp section
        {
            crearmatriz(azul);
            llenarmatriz(azul);
            leerarchivo("azul.txt", azul);
            //saludo();
            //valorinicial(azul);
        }
        #pragma omp section
        {
            crearmatriz(rojo);
            llenarmatriz(rojo);
            leerarchivo("rojo.txt", rojo);
            //saludo();
            //valorinicial(rojo);
        }
        #pragma omp section
        {
            crearmatriz(verde);
            llenarmatriz(verde);
            leerarchivo("verde.txt", verde);
            //saludo();
            //valorinicial(verde);
        }
    }

    return EXIT_SUCCESS;
    
}