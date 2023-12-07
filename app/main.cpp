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

void UnirMatrices(const std::vector<std::vector<double>>& alfa,
                  const std::vector<std::vector<double>>& rojo,
                  const std::vector<std::vector<double>>& azul,
                  const std::vector<std::vector<double>>& verde,
                  std::vector<std::vector<double>>& total) {
    // Asegura que todas las matrices tengan las mismas dimensiones
    if (alfa.size() != rojo.size() || alfa.size() != azul.size() || alfa.size() != verde.size() ||
        alfa[0].size() != rojo[0].size() || alfa[0].size() != azul[0].size() || alfa[0].size() != verde[0].size()) {
        std::cerr << "Las dimensiones de las matrices no son iguales." << std::endl;
        return;
    }

    // Inicializa la matriz total con ceros
    total.resize(alfa.size(), std::vector<double>(alfa[0].size(), 0.0));

    // Combina las matrices asegurándose de que los valores estén en el rango [0, 255]
    for (size_t i = 0; i < alfa.size(); ++i) {
        for (size_t j = 0; j < alfa[i].size(); ++j) {
            total[i][j] = (alfa[i][j] + rojo[i][j] + azul[i][j] + verde[i][j])/4;

            // Ajusta los valores para asegurarse de que estén en el rango [0, 255]
            if (total[i][j] < 0) {
                total[i][j] = 0;
            } else if (total[i][j] > 255) {
                total[i][j] = 255;
            }
        }
    }
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

// Función para generar una imagen a partir de los datos de una matriz
void GenerarImagen(const std::vector<std::vector<double>>& matriz, const char* nombreArchivo) {
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

    // Guardar la imagen en formato bmp
    imagen.save_bmp(nombreArchivo);
}

void LimpiarValoresPerdidos(const std::vector<std::vector<double>>& matrizPromedio,
                   std::vector<std::vector<double>>& matriz) {
    // Asegura que las matrices tengan las mismas dimensiones
    if (matriz.size() != matrizPromedio.size() || matriz[0].size() != matrizPromedio[0].size()) {
        std::cerr << "Las dimensiones de las matrices no son iguales." << std::endl;
        return;
    }
    // Procesa los valores perdidos usando la fórmula del promedio ponderado
    for (size_t i = 0; i < matriz.size(); ++i) {
        for (size_t j = 0; j < matriz[i].size(); ++j) {
            if (matriz[i][j] == -1) {
                // Calcula el valor perdido utilizando la fórmula del promedio ponderado
                matriz[i][j] = matrizPromedio[i][j];
            }
        }
    }
}

int main() {
    // Inicializa las matrices
    std::vector<std::vector<double>> alfa(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> azul(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> rojo(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> verde(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> promedio(filas, std::vector<double>(columnas, 0.0));
    std::vector<std::vector<double>> galaxia(filas, std::vector<double>(columnas, 0.0));

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
            LimpiarValoresPerdidos(promedio, alfa);
            LimpiarValoresPerdidos(promedio, azul);
            LimpiarValoresPerdidos(promedio, rojo);
            LimpiarValoresPerdidos(promedio, verde);
            UnirMatrices(alfa, rojo, azul, verde, galaxia);
            GenerarImagen(galaxia, "galaxia.png");
        }
    }
    return EXIT_SUCCESS;
}