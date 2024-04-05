#include <stdio.h>
#include <stdlib.h>

#define MAX_ROWS 100
#define MAX_COLS 100

// Función para leer datos desde un archivo y cargarlos en una matriz
int readMatrixFromFile(const char* filename, char matrix[MAX_ROWS][MAX_COLS], int *rows, int *cols) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error al abrir el archivo.\n");
        return -1;
    }

    *rows = 0;
    *cols = 0;

    // Leer el archivo línea por línea
    char line[MAX_COLS];
    while (fgets(line, sizeof(line), file) && *rows < MAX_ROWS) {
        // Recorrer cada caracter de la línea
        int col = 0;
        for (int i = 0; line[i] != '\0' && col < MAX_COLS; i++) {
            // Si el caracter es un asterisco o un espacio, se guarda en la matriz
            if (line[i] == '*' || line[i] == ' ' || line[i] == '/' || line[i] == 'x') {
                matrix[*rows][col] = line[i];
                col++;
            }
        }
        // Actualizar el número de columnas si es necesario
        if (col > *cols) {
            *cols = col;
        }
        (*rows)++;
    }

    fclose(file);
    return 0;
}

// Función para imprimir la matriz
void printMatrix(char matrix[MAX_ROWS][MAX_COLS], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    char matrix[MAX_ROWS][MAX_COLS];
    int rows, cols;

    // Nombre del archivo
    const char* filename = "/home/stef/Desktop/Git/SO_S1_2024/laberinto.txt";

    // Leer la matriz desde el archivo
    if (readMatrixFromFile(filename, matrix, &rows, &cols) != 0) {
        printf("Error al leer la matriz desde el archivo.\n");
        return 1;
    }

    // Imprimir la matriz
    printf("Matriz leída desde el archivo:\n");
    printMatrix(matrix, rows, cols);

    return 0;
}
