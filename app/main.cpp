#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <vector>
#include "CImg.h"

const size_t filas = 10681;
const size_t columnas = 7121;

#include <fstream>
#include <iostream>
#include <vector>

void leerarchivo(const std::string& nombreArchivo, std::vector<std::vector<double>>& matriz) {
    std::ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo: " << nombreArchivo << std::endl;
        return;
    }

    // Lee valores del archivo y llena la matriz
    for (size_t i = 0; i < matriz.size(); ++i) {
        for (size_t j = 0; j < matriz[i].size(); ++j) {
            std::string valor;
            archivo >> valor;

            if (valor == "*") {
                // En caso de '*', puedes manejarlo de alguna manera específica
                // Por ahora, simplemente asignamos un valor predeterminado (0.0)
                matriz[i][j] = 0.0;
            } else {
                // Convierte el valor de string a double
                matriz[i][j] = std::stod(valor);
            }
        }
    }

    archivo.close();
}

bool verificarMatriz(const std::vector<std::vector<double>>& matriz) {
    for (size_t i = 0; i < matriz.size(); ++i) {
        for (size_t j = 0; j < matriz[i].size(); ++j) {
            double valor = matriz[i][j];

            if (valor < 0 || valor > 255) {
                // Se encontró un valor fuera del rango
                std::cout << "Valor fuera del rango en la posición (" << i << ", " << j << "): " << valor << std::endl;
                return false;
            }
        }
    }

    // Todos los valores están en el rango [0, 255]
    std::cout << "Todos los valores están en el rango [0, 255]." << std::endl;
    return true;
}

void generarimagen(const std::vector<std::vector<double>>& matriz, const std::string& nombreImagen) {
    // Crea la imagen
    cimg_library::CImg<unsigned char> imagen(columnas, filas, 1, 3);

    // Llena la imagen con los valores de la matriz
    for (size_t i = 0; i < matriz.size(); ++i) {
        for (size_t j = 0; j < matriz[i].size(); ++j) {
            imagen(j, i, 0) = static_cast<unsigned char>(matriz[i][j]);
            imagen(j, i, 1) = static_cast<unsigned char>(matriz[i][j]);
            imagen(j, i, 2) = static_cast<unsigned char>(matriz[i][j]);
        }
    }

    // Guarda la imagen
    imagen.save_png(nombreImagen.c_str());
}

int main() {
    // Inicializa las matrices
    std::vector<std::vector<double>> alfa(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> azul(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> rojo(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> verde(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> promedio(filas, std::vector<double>(columnas, 0.0));

    // Se establece el número de hilos para OpenMP
    omp_set_num_threads(4);

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            leerarchivo("alfa.txt", alfa);
            verificarMatriz(alfa);
        }
        #pragma omp section
        {
            leerarchivo("azul.txt", azul);
            verificarMatriz(azul);
        }
        #pragma omp section
        {
            leerarchivo("rojo.txt", rojo);
            verificarMatriz(rojo);   
        }
        #pragma omp section
        {
            leerarchivo("verde.txt", verde);
            verificarMatriz(verde);
        }
    }

    return EXIT_SUCCESS;
}