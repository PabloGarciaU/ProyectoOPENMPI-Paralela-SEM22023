#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <vector>
#include "CImg.h"

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

void llenarmatriz(std::vector<std::vector<double>>& matriz) {
    #pragma omp parallel for
    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            // Llenar la matriz con cero
            matriz[i][j] = 1.0;
        }
    }
}

void corregirmatriz(){

}

void unirmatriz(){

}

// Función para generar una imagen JPEG a partir de los datos de una matriz
void generarimagen(const std::vector<std::vector<double>>& matriz, const char* nombreArchivo) {
    // Crear una instancia de CImg para representar la imagen
    cimg_library::CImg<unsigned char> imagen(columnas, filas, 1, 3);

    // Mapeo de valores de la matriz a componentes de color y asignación a la imagen
    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            unsigned char valor = static_cast<unsigned char>(matriz[i][j]);
            imagen(j, i, 0) = valor;  // Componente rojo
            imagen(j, i, 1) = valor;  // Componente verde
            imagen(j, i, 2) = valor;  // Componente azul
        }
    }

    // Guardar la imagen en formato PNG
    imagen.save_bmp(nombreArchivo);
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
    // Inicializa las matrices
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
            //llenarmatriz(alfa);
            leerarchivo("alfa.txt", alfa);
            //saludo();
            //valorinicial(alfa);
            generarimagen(alfa, "alfa.png");
        }
        #pragma omp section
        {
            crearmatriz(azul);
            //llenarmatriz(azul);
            leerarchivo("azul.txt", azul);
            //saludo();
            //valorinicial(azul);
            generarimagen(azul, "azul.png");
        }
        #pragma omp section
        {
            crearmatriz(rojo);
            //llenarmatriz(rojo);
            leerarchivo("rojo.txt", rojo);
            //saludo();
            //valorinicial(rojo);
            generarimagen(rojo,"rojo.png");
        }
        #pragma omp section
        {
            crearmatriz(verde);
            //llenarmatriz(verde);
            leerarchivo("verde.txt", verde);
            //saludo();
            //valorinicial(verde);
            generarimagen(verde,"verde.png");

        }
    }

    return EXIT_SUCCESS;

}