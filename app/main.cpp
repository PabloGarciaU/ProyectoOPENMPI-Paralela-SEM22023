#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <vector>
#include "CImg.h"
#include <string>

const size_t filas = 10681;
const size_t columnas = 7121;

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

void LimpiarValoresPerdidos(std::vector<std::vector<double>>& matrizPromedio,
                             std::vector<std::vector<double>>& alfa,
                             std::vector<std::vector<double>>& rojo,
                             std::vector<std::vector<double>>& verde,
                             std::vector<std::vector<double>>& azul) {
    // Asegura que las matrices tengan las mismas dimensiones
    if (matrizPromedio.size() != alfa.size() || matrizPromedio[0].size() != alfa[0].size() ||
        matrizPromedio.size() != rojo.size() || matrizPromedio[0].size() != rojo[0].size() ||
        matrizPromedio.size() != verde.size() || matrizPromedio[0].size() != verde[0].size() ||
        matrizPromedio.size() != azul.size() || matrizPromedio[0].size() != azul[0].size()) {
        std::cerr << "Las dimensiones de las matrices no son iguales." << std::endl;
        return;
    }

    for (size_t i = 0; i < matrizPromedio.size(); ++i) {
        for (size_t j = 0; j < matrizPromedio[i].size(); ++j) {
            if (rojo[i][j] == -1 || verde[i][j] == -1 || azul[i][j] == -1) {
                // Calcula el valor perdido utilizando la fórmula del promedio ponderado
                double valorCalculado = (matrizPromedio[i][j] - alfa[i][j]) / 0.5;

                // Asegurarse de que el valor esté dentro del rango permitido (0 a 255)
                valorCalculado = std::max(0.0, std::min(valorCalculado, 255.0));

                // Actualiza los valores en las matrices rojo, verde y azul
                rojo[i][j] = valorCalculado;
                verde[i][j] = valorCalculado;
                azul[i][j] = valorCalculado;
            }
        }
    }
}

void GenerarImagenColor(const std::vector<std::vector<double>>& alfa,
                        const std::vector<std::vector<double>>& azul,
                        const std::vector<std::vector<double>>& rojo,
                        const std::vector<std::vector<double>>& verde,
                        const std::string& nombreArchivo) {
    // Asegura que las matrices tengan las mismas dimensiones
    if (alfa.size() != azul.size() || alfa[0].size() != azul[0].size() ||
        alfa.size() != rojo.size() || alfa[0].size() != rojo[0].size() ||
        alfa.size() != verde.size() || alfa[0].size() != verde[0].size()) {
        std::cerr << "Las dimensiones de las matrices no son iguales." << std::endl;
        return;
    }

    // Crea una imagen en blanco de 7121x10681
    cimg_library::CImg<unsigned char> imagen(7121, 10681, 1, 3, 0);

    // Recorre las matrices y asigna los valores a la imagen
    for (size_t i = 0; i < alfa.size(); ++i) {
        for (size_t j = 0; j < alfa[i].size(); ++j) {
            imagen(j, i, 0, 0) = rojo[i][j];
            imagen(j, i, 0, 1) = verde[i][j];
            imagen(j, i, 0, 2) = azul[i][j];
        }
    }

    // Guarda la imagen
    imagen.save_bmp(nombreArchivo.c_str());
}

int main() {
    // Inicializa las matrices
    std::vector<std::vector<double>> alfa(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> azul(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> rojo(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> verde(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> promedio(filas, std::vector<double>(columnas, 0.0));

    // Se establece el número de hilos para OpenMP
    omp_set_num_threads(5);

    // Llamada de funciones 
    LeerArchivo("promedio.txt", promedio);
    LeerArchivo("alfa.txt", alfa);
    LeerArchivo("rojo.txt", rojo);
    LeerArchivo("verde.txt", verde);
    LeerArchivo("azul.txt", azul);
    LimpiarValoresPerdidos(promedio, alfa, rojo, verde, azul);
    VerificarMatriz(promedio);
    VerificarMatriz(alfa);
    VerificarMatriz(rojo);
    VerificarMatriz(verde);
    VerificarMatriz(azul);

    // Finalmente con todas las matrices limpiadas y corregidas se genera la imagen final en ARGB
    GenerarImagenColor(alfa, azul, rojo, verde, "galaxia.png");
    return EXIT_SUCCESS;
}