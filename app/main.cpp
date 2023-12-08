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

void LimpiarValoresPerdidos(const std::vector<std::vector<double>>& matrizPromedio,
                             std::vector<std::vector<double>>& rojo,
                             std::vector<std::vector<double>>& verde,
                             std::vector<std::vector<double>>& azul,
                             std::vector<std::vector<double>>& matriz, int caso) {
    // Asegura que las matrices tengan las mismas dimensiones
    if (matriz.size() != matrizPromedio.size() || matriz[0].size() != matrizPromedio[0].size()) {
        std::cerr << "Las dimensiones de las matrices no son iguales." << std::endl;
        return;
    }

    // Procesa los valores perdidos usando la fórmula del promedio ponderado
    for (size_t i = 0; i < matriz.size(); ++i) {
        for (size_t j = 0; j < matriz[i].size(); ++j) {
            if (matriz[i][j] == -1) {
                // Calcula el valor perdido utilizando la fórmula del promedio ponderado con el peso adecuado
                double valorCalculado = 0.0;
                switch (caso) {
                    case 1: // Para el caso de la matriz roja
                        if (azul[i][j] != -1 && verde[i][j] != -1) {
                            valorCalculado = (matrizPromedio[i][j] - (0.59 * verde[i][j]) + (0.11 * azul[i][j])) / 0.3;
                        }
                        break;
                    case 2:// Para el caso de la matriz verde
                        if (rojo[i][j] != -1 && azul[i][j] != -1) {
                            valorCalculado = (matrizPromedio[i][j] - (0.3 * rojo[i][j]) + (0.11 * azul[i][j])) / 0.59;
                        }
                        break;
                    case 3:// para el caso de la matriz azul
                        if (rojo[i][j] != -1 && verde[i][j] != -1) {
                            valorCalculado = (matrizPromedio[i][j] - (0.3 * rojo[i][j]) + (0.59 * verde[i][j])) / 0.11;
                        }
                        break;
                    default:
                        std::cerr << "Caso no reconocido." << std::endl;
                        return;
                }

                // Asegurarse de que el valor esté dentro del rango permitido (0 a 255)
                valorCalculado = std::max(0.0, std::min(valorCalculado, 255.0));
                matriz[i][j] = valorCalculado;
            }
        }
    }
}

void GenerarImagenColor(const std::vector<std::vector<double>>& alfa,
                        const std::vector<std::vector<double>>& azul,
                        const std::vector<std::vector<double>>& rojo,
                        const std::vector<std::vector<double>>& verde,
                        const char* nombreArchivo) {
    // Crea una instancia de CImg para representar la imagen a color
    cimg_library::CImg<unsigned char> imagen(alfa[0].size(), alfa.size(), 1, 3);

    // Mapeo de valores de las matrices a componentes de color y asignación a la imagen
    for (size_t i = 0; i < alfa[0].size(); ++i) {
        for (size_t j = 0; j < alfa.size(); ++j) {
            // Asigna los valores a los componentes de color de la imagen
            imagen(i, j, 0, 0) = static_cast<unsigned char>(rojo[j][i]);   // Componente rojo
            imagen(i, j, 0, 1) = static_cast<unsigned char>(verde[j][i]);  // Componente verde
            imagen(i, j, 0, 2) = static_cast<unsigned char>(azul[j][i]);   // Componente azul
        }
    }

    // Guardar la imagen en formato bmp
    imagen.save_bmp(nombreArchivo);
    std::cout << "Imagen generada, que la disfrute! :) " << nombreArchivo << std::endl;
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
    // Es necesario que antes de paralelizar este valor se carge antes solo, ya que es el pilar de las demas formulas
    LeerArchivo("promedio.txt", promedio);
    LeerArchivo("alfa.txt", alfa);
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            VerificarMatriz(alfa);
        }
        #pragma omp section
        {
            LeerArchivo("rojo.txt", rojo);
            LimpiarValoresPerdidos(promedio, rojo, verde, azul, rojo, 1);
            VerificarMatriz(rojo);
            
        }
        #pragma omp section
        {
            LeerArchivo("verde.txt", verde);
            LimpiarValoresPerdidos(promedio, rojo, verde, azul, verde, 2);
            VerificarMatriz(verde);
        }
        #pragma omp section
        {
            LeerArchivo("azul.txt", azul);
            LimpiarValoresPerdidos(promedio, rojo, verde, azul, azul, 3);
            VerificarMatriz(azul);
        }
    }
    // Finalmente con todas las matrices limpiadas y corregidas se genera la imagen final en ARGB
    GenerarImagenColor(alfa, azul, rojo, verde, "galaxia.png");
    return EXIT_SUCCESS;
}