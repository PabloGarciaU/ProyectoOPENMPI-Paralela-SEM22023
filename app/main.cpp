#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <vector>
#include "CImg.h"
#include <string>

const size_t filas = 7121;
const size_t columnas = 10681;

void LeerArchivo(const std::string& nombreArchivo, std::vector<std::vector<int>>& matriz) {
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
                // Convierte el valor de string a int
                matriz[i][j] = std::stoi(valor);
            }
        }
    }

    archivo.close();
}

bool VerificarMatriz(const std::vector<std::vector<int>>& matriz) {
    for (size_t i = 0; i < matriz.size(); ++i) {
        for (size_t j = 0; j < matriz[i].size(); ++j) {
            int valor = matriz[i][j];

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

void LimpiarValoresPerdidosRojo(std::vector<std::vector<int>>& matrizPromedio,
                                std::vector<std::vector<int>>& alfa,
                                std::vector<std::vector<int>>& rojo,
                                std::vector<std::vector<int>>& verde,
                                std::vector<std::vector<int>>& azul) {
    // Asegura que las matrices tengan las mismas dimensiones
    if (matrizPromedio.size() != alfa.size() || matrizPromedio[0].size() != alfa[0].size() ||
        matrizPromedio.size() != rojo.size() || matrizPromedio[0].size() != rojo[0].size() ||
        matrizPromedio.size() != verde.size() || matrizPromedio[0].size() != verde[0].size() ||
        matrizPromedio.size() != azul.size() || matrizPromedio[0].size() != azul[0].size()) {
        std::cerr << "Las dimensiones de las matrices no son iguales." << std::endl;
        return;
    }

    // Procesa los valores perdidos para la matriz rojo
    for (size_t i = 0; i < matrizPromedio.size(); ++i) {
        for (size_t j = 0; j < matrizPromedio[0].size(); ++j) {
            if (rojo[i][j] < 0) {
                // Calcula el valor perdido en la matriz rojo utilizando la fórmula
                double valorCalculadoRojo = (alfa[i][j] != -1) ? (matrizPromedio[i][j] - (0.59 * verde[i][j]) + (0.11 * azul[i][j])) / 0.3 : 0;

                // Asegura que el valor esté dentro del rango permitido (0 a 255)
                valorCalculadoRojo = std::max(0.0, std::min(valorCalculadoRojo, 255.0));

                // Asigna el valor calculado a la matriz rojo
                rojo[i][j] = static_cast<int>(valorCalculadoRojo);
            }
        }
    }
}

void LimpiarValoresPerdidosVerde(std::vector<std::vector<int>>& matrizPromedio,
                                 std::vector<std::vector<int>>& alfa,
                                 std::vector<std::vector<int>>& rojo,
                                 std::vector<std::vector<int>>& verde,
                                 std::vector<std::vector<int>>& azul) {
    // Asegura que las matrices tengan las mismas dimensiones
    if (matrizPromedio.size() != alfa.size() || matrizPromedio[0].size() != alfa[0].size() ||
        matrizPromedio.size() != rojo.size() || matrizPromedio[0].size() != rojo[0].size() ||
        matrizPromedio.size() != verde.size() || matrizPromedio[0].size() != verde[0].size() ||
        matrizPromedio.size() != azul.size() || matrizPromedio[0].size() != azul[0].size()) {
        std::cerr << "Las dimensiones de las matrices no son iguales." << std::endl;
        return;
    }

    // Procesa los valores perdidos para la matriz verde
    for (size_t i = 0; i < matrizPromedio.size(); ++i) {
        for (size_t j = 0; j < matrizPromedio[0].size(); ++j) {
            if (verde[i][j] < 0) {
                // Calcula el valor perdido en la matriz verde utilizando la fórmula
                double valorCalculadoVerde = (alfa[i][j] != -1) ? (matrizPromedio[i][j] - (0.3 * rojo[i][j]) + (0.11 * azul[i][j])) / 0.59 : 0;

                // Asegura que el valor esté dentro del rango permitido (0 a 255)
                valorCalculadoVerde = std::max(0.0, std::min(valorCalculadoVerde, 255.0));

                // Asigna el valor calculado a la matriz verde
                verde[i][j] = static_cast<int>(valorCalculadoVerde);
            }
        }
    }
}

void LimpiarValoresPerdidosAzul(std::vector<std::vector<int>>& matrizPromedio,
                                std::vector<std::vector<int>>& alfa,
                                std::vector<std::vector<int>>& rojo,
                                std::vector<std::vector<int>>& verde,
                                std::vector<std::vector<int>>& azul) {
    // Asegura que las matrices tengan las mismas dimensiones
    if (matrizPromedio.size() != alfa.size() || matrizPromedio[0].size() != alfa[0].size() ||
        matrizPromedio.size() != rojo.size() || matrizPromedio[0].size() != rojo[0].size() ||
        matrizPromedio.size() != verde.size() || matrizPromedio[0].size() != verde[0].size() ||
        matrizPromedio.size() != azul.size() || matrizPromedio[0].size() != azul[0].size()) {
        std::cerr << "Las dimensiones de las matrices no son iguales." << std::endl;
        return;
    }

    // Procesa los valores perdidos para la matriz azul
    for (size_t i = 0; i < matrizPromedio.size(); ++i) {
        for (size_t j = 0; j < matrizPromedio[0].size(); ++j) {
            if (azul[i][j] < 0) {
                // Calcula el valor perdido en la matriz azul utilizando la fórmula
                double valorCalculadoAzul = (alfa[i][j] != -1) ? (matrizPromedio[i][j] - (0.3 * rojo[i][j]) + (0.59 * verde[i][j])) / 0.11 : 0;

                // Asegura que el valor esté dentro del rango permitido (0 a 255)
                valorCalculadoAzul = std::max(0.0, std::min(valorCalculadoAzul, 255.0));

                // Asigna el valor calculado a la matriz azul
                azul[i][j] = static_cast<int>(valorCalculadoAzul);
            }
        }
    }
}

void GenerarImagenColor(const std::vector<std::vector<int>>& alfa,
                        const std::vector<std::vector<int>>& azul,
                        const std::vector<std::vector<int>>& rojo,
                        const std::vector<std::vector<int>>& verde,
                        const std::string& nombreArchivo) {
    // Asegura que las matrices tengan las mismas dimensiones
    if (alfa.size() != azul.size() || alfa[0].size() != azul[0].size() ||
        alfa.size() != rojo.size() || alfa[0].size() != rojo[0].size() ||
        alfa.size() != verde.size() || alfa[0].size() != verde[0].size()) {
        std::cerr << "Las dimensiones de las matrices no son iguales." << std::endl;
        return;
    }

    // Crea una imagen en blanco de filas x columnas
    cimg_library::CImg<unsigned char> imagen(alfa[0].size(), alfa.size(), 1, 3, 0);

    // Recorre las matrices y asigna los valores a la imagen
    for (size_t i = 0; i < alfa.size(); ++i) {
        for (size_t j = 0; j < alfa[i].size(); ++j) {
            // Asegura que los valores estén en el rango [0, 255]
            imagen(j, i, 0, 0) = static_cast<unsigned char>(rojo[i][j]);
            imagen(j, i, 0, 1) = static_cast<unsigned char>(verde[i][j]);
            imagen(j, i, 0, 2) = static_cast<unsigned char>(azul[i][j]);
        }
    }

    // Guarda la imagen
    imagen.save_bmp(nombreArchivo.c_str());
    // Señala con un mensaje que la imagen se genero con exito
    std::cout << "Imagen generada con exito, disfrute de su galaxia ;)." << std::endl;
}

int main() {
    // Inicializa las matrices
    std::vector<std::vector<int>> promedio(filas, std::vector<int>(columnas, 0));
    std::vector<std::vector<int>> alfa(filas, std::vector<int>(columnas, 0));
    std::vector<std::vector<int>> rojo(filas, std::vector<int>(columnas, 0));
    std::vector<std::vector<int>> verde(filas, std::vector<int>(columnas, 0));
    std::vector<std::vector<int>> azul(filas, std::vector<int>(columnas, 0));

    // Establece el número de hilos para las siguientes secciones
    omp_set_num_threads(4);

    // Paraleliza la lectura de archivos
    #pragma omp parallel sections   
    {
        #pragma omp section
        {
            LeerArchivo("promedio.txt", promedio);
            LeerArchivo("alfa.txt", alfa);
        }

        #pragma omp section
        {
            LeerArchivo("rojo.txt", rojo);
        }

        #pragma omp section
        {
            LeerArchivo("verde.txt", verde);
        }

        #pragma omp section
        {
            LeerArchivo("azul.txt", azul);
        }
    }

    // Establece el número de hilos para las siguientes secciones
    omp_set_num_threads(4);

    // Paraleliza las secciones de procesamiento
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            LimpiarValoresPerdidosRojo(promedio, alfa, rojo, verde, azul);
            VerificarMatriz(rojo);
        }

        #pragma omp section
        {
            LimpiarValoresPerdidosVerde(promedio, alfa, rojo, verde, azul);
            VerificarMatriz(verde);
        }

        #pragma omp section
        {
            LimpiarValoresPerdidosAzul(promedio, alfa, rojo, verde, azul);
            VerificarMatriz(azul);
        }

        #pragma omp section
        {
            GenerarImagenColor(alfa, azul, rojo, verde, "galaxia.png");
        }
    }
    return EXIT_SUCCESS;    
}