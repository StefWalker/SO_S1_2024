#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h> 

#define MAX_ROWS 100
#define MAX_COLS 100

//PROTOTIPOS
void *traverseMatrix(void *arg);

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

// Estructura para los argumentos del hilo
typedef struct {
    char matrix[MAX_ROWS][MAX_COLS];
    int rows;
    int cols;
    int x ; // Posición inicial en x
    int y ; // Posición inicial en y
    int dx; // Dirección de movimiento en x (1 para la derecha, -1 para la izquierda)
    int dy;
    char simbolo; 
} ThreadArgs;


void createNewThread(ThreadArgs *args,char new_simbol) {
    pthread_t thread_id;
    //printf("Creating new thread to continue in direction (%d, %d)\n", args->dx, args->dy);
    
    // Copiar la matriz al argumento del hilo
    ThreadArgs new_args = *args;
    new_args.simbolo = new_simbol;
    
    pthread_create(&thread_id, NULL, traverseMatrix, (void *)&new_args);
    pthread_join(thread_id, NULL);
}

// Función que el hilo ejecutará
void *traverseMatrix(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    //int x = 0; // Posición inicial en x
    //int y = 0; // Posición inicial en y
    //int dx = 1; // Dirección de movimiento en x (1 para la derecha, -1 para la izquierda)
    //int dy = 0; // Dirección de movimiento en y (1 para abajo, -1 para arriba)

    // Recorrer la matriz
    while (1) {
        // Poner 'x' en la casilla actual
        args->matrix[args->y][args->x] = args->simbolo;

        // Comprobar si estamos fuera de los límites de la matriz
        if (args->y == args->rows){
            //tratar de ir hacia derecha
            printf("llegue al final de abajo");
            args->dx = 1;
            args->dy = 0;
            args->matrix[args->y][args->x] = args->simbolo;
            args->x = args->x+1;
            createNewThread(args,'O');

            break;
        }
        if (args->x == args->cols) {
            //tratar de ir hacie abajo
            args->dx = 0;
            args->dy = 1;
            args->matrix[args->y][args->x] = args->simbolo;
            args->x = args->y+1;
            createNewThread(args,'C');
            break;;
        }
        if(args->y-1 < 0){


        }

            
        if (args->dx != 0) { // Si estábamos moviéndonos horizontalmente
        // Comprobar si encontramos una pared ('*')
            if (args->matrix[args->y][args->x+1] == '*') {
                // Cambiar la dirección de movimiento
                
                    // Intentar moverse hacia abajo
                    args->dx = 0;
                    args->dy = 1;
                    args->matrix[args->y][args->x] = args->simbolo;
                    args->y = args->y +1;
                    createNewThread(args,'C');
                    return NULL;
            
                
            }
        }
       
        if (args->dy != 0) { // Si estábamos moviéndonos verticalmente
            if (args->matrix[args->y+1][args->x] == '*') {
                // Cambiar la dirección de movimiento
                
                // Intentar moverse hacia la derecha
                args->dx = 1;
                args->dy = 0;
                args->matrix[args->y][args->x] = args->simbolo;
                args->x = args->x+1;
                createNewThread(args,'O');


                return NULL;
            } 
        }
        
    
     
        // Imprimir la matriz modificada
        system("clear"); // Limpiar la pantalla (compatible con sistemas Unix)
        printf("Matriz modificada:\n");
        printMatrix(args->matrix, args->rows, args->cols);
        sleep(1); // Esperar un segundo antes de avanzar

        // Moverse en la dirección actual
        args->x += args->dx;
        args->y += args->dy;

    }

    return NULL;
}



int main() {
    char matrix[MAX_ROWS][MAX_COLS];
    int rows, cols;

    // Nombre del archivo
     const char* filename = "/Users/admin/Documents/1 Semestre 2024/SO/SO_S1_2024/laberinto.txt";

    // Leer la matriz desde el archivo
    if (readMatrixFromFile(filename, matrix, &rows, &cols) != 0) {
        printf("Error al leer la matriz desde el archivo.\n");
        return 1;
    }

    // Imprimir la matriz
    printf("Matriz leída desde el archivo:\n");
    printMatrix(matrix, rows, cols);

    // Crear la estructura de argumentos para el hilo
    ThreadArgs args = {
        .rows = rows,
        .cols = cols,
        .x = 0, // Posición inicial en x
        .y = 0, // Posición inicial en y
        .dx = 1, // Dirección de movimiento en x (1 para la derecha, -1 para la izquierda)
        .dy = 0,
        .simbolo = 'X' 
    };

    // Copiar la matriz al argumento del hilo
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            args.matrix[i][j] = matrix[i][j];
        }
    }

    pthread_t thread_id;
    printf("Before Thread\n");
    pthread_create(&thread_id, NULL, traverseMatrix, (void *)&args);
    pthread_join(thread_id, NULL);
    printf("After Thread\n");
    return 0;
}
