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

void LeerArchivo(const std::string& nombreArchivo, std::vector<std::vector<double>>& matriz) {
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
                // Reemplaza '*' con -1
                matriz[i][j] = -1;
            } else {
                // Convierte el valor de string a double
                matriz[i][j] = std::stod(valor);
            }
        }
    }

    archivo.close();
}

bool VerificarMatriz(const std::vector<std::vector<double>>& matriz) {
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

void GenerarImagen(const std::vector<std::vector<double>>& matriz, const std::string& nombreImagen) {
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

    // Guarda la imagen en formato PNG
    imagen.save_png(nombreImagen.c_str());
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

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            LeerArchivo("promedio.txt", promedio);
            LeerArchivo("alfa.txt", alfa);
            LeerArchivo("azul.txt", azul);
            LeerArchivo("rojo.txt", rojo);
            LeerArchivo("verde.txt", verde);
            VerificarMatriz(promedio);
            VerificarMatriz(alfa);
            VerificarMatriz(azul);
            VerificarMatriz(rojo);
            VerificarMatriz(verde);
            GenerarImagen(promedio, "promedio.png");
        }
    }
    return EXIT_SUCCESS;
}