// Principios de Sistemas Operativos IC-6600
//   Instituto Tecnológico de Costa Rica
//
//         Dayron Padilla Cubillo
//          Dylan Torres Walker 
// 
//              Proyecto 1
//          Laberinto Threads

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

// Definimos tamaño de la matriz
#define ROWS 8
#define COLS 8

int leer_matrix_file(const char* filename, char matrix[ROWS][COLS]);

// Nombre del archivo
const char* filename = "laberinto.txt";

// Definición del laberinto base con los chars designados
char laberinto[ROWS][COLS];

// Matriz para registrar celdas visitadas por cada hilo
int visitado[ROWS][COLS] = {0};

// Mutex para sincronización
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Función para leer datos desde un archivo y cargarlos en una matriz
int leer_matrix_file(const char* filename, char matrix[ROWS][COLS]) {
    int rows = 0, cols = 0; // Inicializar las variables para el número de filas y columnas
    int MAX_ROWS = 100, MAX_COLS = 100;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error al abrir el archivo.\n");
        return -1;
    }
    char line[MAX_COLS];
    while (fgets(line, sizeof(line), file) && rows < MAX_ROWS) {
        int col = 0;
        for (int i = 0; line[i] != '\0' && col < MAX_COLS; i++) {
            // Del archivo se leen solo los siguientes caracteres, esto permite tener las ',' como separadores
            if (line[i] == '*' || line[i] == ' ' || line[i] == '/' || line[i] == 'x'){
                matrix[rows][col] = line[i]; // Almacenamos datos en la matriz
                col++;
            }
        }
        if (col > cols) {
            cols = col;
        }
        (rows)++;
    }
    fclose(file);
    return 0;
}

// Estructura para pasar información a los hilos
typedef struct {
    int row;
    int col;
    int pasos;
    int direccion;
} ThreadArgs;

// Función para mostrar el laberinto
void print_laberinto() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c ", laberinto[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Función para validar si una posición es válida
int is_valid(int row, int col) {
    return row >= 0 && row < ROWS && col >= 0 && col < COLS && laberinto[row][col] != '*' && !visitado[row][col];
}

// Función para recorrido_threadr un hilo
void recorrido_thread(ThreadArgs *args) {
    int row = args->row;
    int col = args->col;
    int pasos = args->pasos;
    int direccion = args->direccion;
    int contador = 1;
    bool activo = true;

    while(activo){
        // Marcar la posición actual como visitada
        visitado[row][col] = 1;
        // Utilizamos mutex para evitar el choque de datos cada vez que un hilo quiera cammbiar la cantidad de pasos y imprime la matriz
        pthread_mutex_lock(&mutex); 
        //system("clear");
        printf("Cantidad de pasos: %d\n", pasos);
        print_laberinto();
        pthread_mutex_unlock(&mutex);
        // Simulacion de datos
        sleep(1); 

        // Verificar la salida
        if (laberinto[row][col] == '/') {
            printf("Hilo exitoso! Pasos totales: %d\n", pasos);
            pthread_exit(NULL);
        }

        // Marcar la posición actual en el laberinto
        laberinto[row][col] = 'x';

        // Direcciones posibles
        // d_row -1: izquierda
        // d_row 1:  derecha
        // d_col -1: arriba
        // d_col 1:  abajo
        int d_row[] = {-1, 1, 0, 0};
        int d_col[] = {0, 0, -1, 1};
    
        // Explorar las direcciones
        for (int i = 0; i < 4; i++) {
            int new_row = row + d_row[i];
            int new_col = col + d_col[i];

            if (is_valid(new_row, new_col)) {
                // Crear un nuevo hilo
                // Con sus respectivos cambios de direccion
                if(i == direccion){
                    row = new_row;
                    col = new_col;
                    pasos ++;
                    contador ++;
                }
                else{
                    ThreadArgs *new_args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
                    new_args->row = new_row;
                    new_args->col = new_col;
                    new_args->pasos = pasos + 1;
                    new_args->direccion = i;

                    pthread_t thread;
                    pthread_create(&thread, NULL, (void *(*)(void *))recorrido_thread, new_args);
                }
            }
            if(!is_valid(new_row, new_col) && direccion == i) {
                activo = false;
            }
        }
    }
    // Finalizamos el hilo
    printf("Finalizo hilo con direccion %d y pasos totales de este hilo: %d\n", direccion, contador);
    pthread_exit(NULL);
}

int main() {
    // Leemos el archivo txt que contiene el laberinto
    if (leer_matrix_file(filename,laberinto) != 0) {
        printf("Error al leer la matriz desde el archivo.\n");
        return 1;
    }
    if(laberinto[0][0] == ' ')
    {
        // Iniciar el movimiento desde la posición (0, 0)
        ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        args->row = 0;
        args->col = 0;
        args->pasos = 0;
        args->direccion = 0;

        recorrido_thread(args);
    }
    return 0;
}


