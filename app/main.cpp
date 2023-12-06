#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <vector>
#include "CImg.h"

const std::vector<std::vector<double>>::size_type filas = 10681;
const std::vector<double>::size_type columnas = 7121;

void leerarchivo(const std::string& nombreArchivo, std::vector<std::vector<double>>& matriz, const std::vector<std::vector<double>>& matrizPromedio) {
    std::ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo: " << nombreArchivo << std::endl;
        return;
    }

    // Lee valores del archivo y llena la matriz
    #pragma omp parallel for collapse(2) shared(matriz, matrizPromedio)
    for (std::vector<std::vector<double>>::size_type i = 0; i < matriz.size(); ++i) {
        for (std::vector<double>::size_type j = 0; j < matriz[i].size(); ++j) {
            std::string valor;
            archivo >> valor;

            if (valor == "*") {
                // Reemplaza '*' con el valor correspondiente en el archivo promedio
                matriz[i][j] = matrizPromedio[i][j];
            } else {
                // Convierte el valor de string a double
                matriz[i][j] = std::stod(valor);
            }
        }
    }

    archivo.close();
}

void generarimagen(const std::vector<std::vector<double>>& matriz, const char* nombreArchivo) {
    cimg_library::CImg<unsigned char> imagen(matriz[0].size(), matriz.size(), 1, 3);

    // Mapeo de valores de la matriz a componentes de color y asignación a la imagen
    #pragma omp parallel for collapse(2) shared(matriz)
    for (std::vector<std::vector<double>>::size_type i = 0; i < matriz.size(); ++i) {
        for (std::vector<double>::size_type j = 0; j < matriz[0].size(); ++j) {
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

    // Guardar la imagen en formato png
    imagen.save_png(nombreArchivo);
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
            leerarchivo("promedio.txt", promedio, promedio);
            generarimagen(promedio, "promedio.png");
        }
        #pragma omp section
        {
            leerarchivo("alfa.txt", alfa, promedio);
            generarimagen(alfa, "alfa.png");
        }
        #pragma omp section
        {
            leerarchivo("azul.txt", azul, promedio);
            generarimagen(azul, "azul.png");
        }
        #pragma omp section
        {
            leerarchivo("rojo.txt", rojo, promedio);
            generarimagen(rojo, "rojo.png");    
        }
        #pragma omp section
        {
            leerarchivo("verde.txt", verde, promedio);
            generarimagen(verde, "verde.png");
        }
    }

    return EXIT_SUCCESS;
}