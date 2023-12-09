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

void LimpiarValoresPerdidos(std::vector<std::vector<int>>& matrizPromedio,
                             std::vector<std::vector<int>>& alfa,
                             std::vector<std::vector<int>>& rojo,
                             std::vector<std::vector<int>>& verde,
                             std::vector<std::vector<int>>& azul, int caso) {
    // Asegura que las matrices tengan las mismas dimensiones
    if (matrizPromedio.size() != alfa.size() || matrizPromedio[0].size() != alfa[0].size() ||
        matrizPromedio.size() != rojo.size() || matrizPromedio[0].size() != rojo[0].size() ||
        matrizPromedio.size() != verde.size() || matrizPromedio[0].size() != verde[0].size() ||
        matrizPromedio.size() != azul.size() || matrizPromedio[0].size() != azul[0].size()) {
        std::cerr << "Las dimensiones de las matrices no son iguales." << std::endl;
        return;
    }

    // Procesa los valores perdidos usando la fórmula correspondiente al caso
    for (size_t i = 0; i < matrizPromedio.size(); ++i) {
        for (size_t j = 0; j < matrizPromedio[0].size(); ++j) {
            if (rojo[i][j] < 0 || verde[i][j] < 0 || azul[i][j] < 0) {
                // Calcula el valor perdido según el caso
                double valorCalculadoRojo = 0;
                double valorCalculadoVerde = 0;
                double valorCalculadoAzul = 0;

                switch (caso) {
                    case 1: // Para el caso de la matriz roja
                        if (azul[i][j] != -1 && verde[i][j] != -1) {
                            valorCalculadoRojo = (matrizPromedio[i][j] - (0.59 * verde[i][j]) + (0.11 * azul[i][j])) / 0.3;
                        }
                        else valorCalculadoRojo = matrizPromedio[i][j];
                        break;
                    case 2:// Para el caso de la matriz verde
                        if (rojo[i][j] != -1 && azul[i][j] != -1) {
                            valorCalculadoVerde = (matrizPromedio[i][j] - (0.3 * rojo[i][j]) + (0.11 * azul[i][j])) / 0.59;
                        }
                        else valorCalculadoVerde = matrizPromedio[i][j];
                        break;
                    case 3:// para el caso de la matriz azul
                        if (rojo[i][j] != -1 && verde[i][j] != -1) {
                            valorCalculadoAzul = (matrizPromedio[i][j] - (0.3 * rojo[i][j]) + (0.59 * verde[i][j])) / 0.11;
                        }
                        else valorCalculadoAzul = matrizPromedio[i][j];
                        break;
                    default:
                        std::cerr << "Caso no reconocido." << std::endl;
                        return;
                }

                // Asegurarse de que los valores estén dentro del rango permitido (0 a 255)
                valorCalculadoRojo = std::max(0.0, std::min(valorCalculadoRojo, 255.0));
                valorCalculadoVerde = std::max(0.0, std::min(valorCalculadoVerde, 255.0));
                valorCalculadoAzul = std::max(0.0, std::min(valorCalculadoAzul, 255.0));

                // Asigna los valores calculados a las matrices correspondientes
                rojo[i][j] = (rojo[i][j] < 0) ? static_cast<int>(valorCalculadoRojo) : rojo[i][j];
                verde[i][j] = (verde[i][j] < 0) ? static_cast<int>(valorCalculadoVerde) : verde[i][j];
                azul[i][j] = (azul[i][j] < 0) ? static_cast<int>(valorCalculadoAzul) : azul[i][j];
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
    cimg_library::CImg<unsigned char> imagen(filas, columnas, 1, 3, 0);

    // Recorre las matrices y asigna los valores a la imagen
    for (size_t i = 0; i < alfa.size(); ++i) {
        for (size_t j = 0; j < alfa[i].size(); ++j) {
            // Asegura que los valores estén en el rango [0, 255]
            imagen(i, j, 0, 0) = static_cast<unsigned char>(std::max(0, std::min(255, rojo[i][j])));
            imagen(i, j, 0, 1) = static_cast<unsigned char>(std::max(0, std::min(255, verde[i][j])));
            imagen(i, j, 0, 2) = static_cast<unsigned char>(std::max(0, std::min(255, azul[i][j])));
        }
    }

    // Guarda la imagen
    imagen.save_bmp(nombreArchivo.c_str());
}

int main() {
    // Inicializa las matrices
    std::vector<std::vector<int>> alfa(filas, std::vector<int>(columnas, 0));
    std::vector<std::vector<int>> azul(filas, std::vector<int>(columnas, 0));
    std::vector<std::vector<int>> rojo(filas, std::vector<int>(columnas, 0));
    std::vector<std::vector<int>> verde(filas, std::vector<int>(columnas, 0));
    std::vector<std::vector<int>> promedio(filas, std::vector<int>(columnas, 0));
 
    // Llamada de todas las funciones, despues se paralelizara

    LeerArchivo("promedio.txt", promedio);
    LeerArchivo("alfa.txt", alfa);
    LeerArchivo("rojo.txt", rojo);
    LeerArchivo("verde.txt", verde);
    LeerArchivo("azul.txt", azul);

    omp_set_num_threads(4);

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            LimpiarValoresPerdidos(promedio, alfa, rojo, verde, azul, 1); // Limpia los valores de la matriz rojo
            VerificarMatriz(rojo);
        }

        #pragma omp section
        {
            LimpiarValoresPerdidos(promedio, alfa, rojo, verde, azul, 2); // Limpia los valores de la matriz verde
            VerificarMatriz(verde);
        }

        #pragma omp section
        {
            LimpiarValoresPerdidos(promedio, alfa, rojo, verde, azul, 3); // Limpia los valores de la matriz azul
            VerificarMatriz(azul);
        }
        #pragma omp section
        {
            GenerarImagenColor(alfa, azul, rojo, verde, "galaxia.png");
        }
    }

    return EXIT_SUCCESS;

}