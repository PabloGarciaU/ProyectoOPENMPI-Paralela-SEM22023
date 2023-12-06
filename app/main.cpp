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

void corregirmatriz(std::vector<std::vector<double>>& matriz, const std::vector<std::vector<double>>& promedio) {
    // Reemplazar los "*" en la matriz con los datos de promedio
    #pragma omp parallel for
    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            if (matriz[i][j] == '*') {
                // Reemplazar "*" con el valor correspondiente de promedio
                matriz[i][j] = promedio[i][j];
            }
        }
    }
}

void verificarMatriz(const std::vector<std::vector<double>>& matriz) {
    bool matrizValida = true;

    // Verificar cada elemento de la matriz
    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            double valor = matriz[i][j];

            // Verificar si el valor está fuera del rango [0, 255]
            if (valor < 0.0 || valor > 255.0) {
                matrizValida = false;
                break;
            }
        }

        // Si se encuentra un valor fuera del rango, salir del bucle externo
        if (!matrizValida) {
            break;
        }
    }

    // Imprimir el mensaje correspondiente
    if (matrizValida) {
        std::cout << "Matriz verificada" << std::endl;
    } else {
        std::cout << "Matriz con errores" << std::endl;
    }
}

// Función para leer el archivo de promedio
void leerarchivo(const std::string& nombreArchivo, std::vector<std::vector<double>>& matriz) {
    std::ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo: " << nombreArchivo << std::endl;
        return;
    }

    // Lee valores del archivo y llena la matriz
    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            archivo >> matriz[i][j];
        }
    }

    archivo.close();
}

// Función para generar una imagen JPEG a partir de los datos de una matriz
void generarimagen(const std::vector<std::vector<double>>& matriz, const char* nombreArchivo) {
    // Crear una instancia de CImg para representar la imagen
    cimg_library::CImg<unsigned char> imagen(matriz[0].size(), matriz.size(), 1, 3);

    // Mapeo de valores de la matriz a componentes de color y asignación a la imagen
    for (size_t i = 0; i < matriz.size(); ++i) {
        for (size_t j = 0; j < matriz[0].size(); ++j) {
            // Asegúrate de que los valores estén en el rango [0, 255]
            double valor = matriz[i][j];
            valor = std::min(std::max(valor, 0.0), 255.0);

            // Escala los valores al rango [0, 255]
            valor = (valor / 255.0) * 255.0;

            // Asigna el valor a los componentes de color de la imagen
            imagen(j, i, 0) = static_cast<unsigned char>(valor);  // Componente rojo
            imagen(j, i, 1) = static_cast<unsigned char>(valor);  // Componente verde
            imagen(j, i, 2) = static_cast<unsigned char>(valor);  // Componente azul
        }
    }

    // Guardar la imagen en formato BMP
    imagen.save_bmp(nombreArchivo);
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
    std::vector<std::vector<double>> promedio(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> total(filas, std::vector<double>(columnas, 0.0)); 

    // Se establece el número de hilos para OpenMP
    omp_set_num_threads(4);

    // Leer el archivo de promedio , verificar, y asignar valores a la matriz promedio
    leerarchivo("promedio.txt", promedio);
    verificarMatriz(promedio);

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            // Operaciones con alfa
            crearmatriz(alfa);
            //llenarmatriz(alfa);
            leerarchivo("alfa.txt", alfa);
            //saludo();
            //valorinicial(alfa);
            corregirmatriz(alfa, promedio);
            verificarMatriz(alfa);
            //generarimagen(alfa, "alfa.png");
        }
        #pragma omp section
        {
            // Operaciones con azul
            crearmatriz(azul);
            //llenarmatriz(azul);
            leerarchivo("azul.txt", azul);
            //saludo();
            //valorinicial(azul);
            corregirmatriz(azul, promedio);
            verificarMatriz(azul);
            //generarimagen(azul, "azul.png");
        }
        #pragma omp section
        {
            // Operaciones con rojo
            crearmatriz(rojo);
            //llenarmatriz(rojo);
            leerarchivo("rojo.txt", rojo);
            //saludo();
            //valorinicial(rojo);
            corregirmatriz(rojo, promedio);
            verificarMatriz(rojo);
            //generarimagen(rojo,"rojo.png");
        }
        #pragma omp section
        {
            // Operaciones con verde
            crearmatriz(verde);
            //llenarmatriz(verde);
            leerarchivo("verde.txt", verde);
            //saludo();
            //valorinicial(verde);
            corregirmatriz(verde, promedio);
            verificarMatriz(verde);
            //generarimagen(verde,"verde.png");
        }
    }

    return EXIT_SUCCESS;
}