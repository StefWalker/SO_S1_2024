#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


#define ROWS 9
#define COLS 9
int readMatrixFromFile(const char* filename, char matrix[ROWS][COLS]);


// Nombre del archivo
const char* filename = "/Users/admin/Documents/1 Semestre 2024/SO/SO_S1_2024/laberinto.txt";


// Definición del laberinto
char maze[ROWS][COLS];
//= 
// {
//     {' ', '*', ' ', ' ', ' ', '*', '*', '*'},
//     {' ', '*', ' ', '*', ' ', ' ', ' ', ' '},
//     {' ', '*', ' ', '*', ' ', '*', '*', '*'},
//     {' ', '*', ' ', '*', '*', '*', '*', '*'},
//     {' ', ' ', ' ', '*', ' ', ' ', ' ', '*'},
//     {'*', '*', ' ', '*', ' ', '*', ' ', ' '},
//     {' ', '*', ' ', '*', ' ', '*', ' ', ' '},
//     {' ', ' ', ' ', ' ', ' ', '*', '/', ' '}
// };

// Matriz para registrar celdas visitadas por cada hilo
int visited[ROWS][COLS] = {0};

// Mutex para sincronización
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Estructura para pasar información a los hilos
typedef struct {
    int row;
    int col;
    int steps;
    int mark;
} ThreadArgs;

// Función para mostrar el laberinto
void print_maze() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c ", maze[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Función para validar si una posición es válida
int is_valid(int row, int col) {
    return row >= 0 && row < ROWS && col >= 0 && col < COLS && maze[row][col] != '*' && !visited[row][col];
}

// Función para mover un hilo
void move(ThreadArgs *args) {
    int row = args->row;
    int col = args->col;
    int steps = args->steps;

    // Marcar la posición actual como visitada
    visited[row][col] = 1;

    // Marcar la posición actual en el laberinto
    maze[row][col] = 'x';

    // Mostrar el laberinto
    pthread_mutex_lock(&mutex);
    printf("Hilo en la posición (%d, %d), pasos: %d\n", row, col, steps);
    system("clear");
    print_maze();
    pthread_mutex_unlock(&mutex);
    sleep(1);

    // Verificar la salida
    if (maze[row][col] == '/') {
        printf("Hilo exitoso! Pasos totales: %d\n", steps);
        pthread_exit(NULL);
    }

    // Direcciones posibles
    int d_row[] = {-1, 1, 0, 0};
    int d_col[] = {0, 0, -1, 1};

    // Explorar las direcciones
    for (int i = 0; i < 4; i++) {
        int new_row = row + d_row[i];
        int new_col = col + d_col[i];

        if (is_valid(new_row, new_col)) {
            // Crear un nuevo hilo
            ThreadArgs *new_args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
            new_args->row = new_row;
            new_args->col = new_col;
            new_args->steps = steps + 1;

            pthread_t thread;
            pthread_create(&thread, NULL, (void *(*)(void *))move, new_args);
        }
    }

    // No necesitamos esperar a que los nuevos hilos terminen antes de finalizar este hilo
    pthread_exit(NULL);
}

int main() {
    if (readMatrixFromFile(filename,maze) != 0) {
        printf("Error al leer la matriz desde el archivo.\n");
        return 1;
    }
    // Iniciar el movimiento desde la posición (0, 0)
    ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
    args->row = 0;
    args->col = 0;
    args->steps = 0;

    move(args);

    return 0;
}


int readMatrixFromFile(const char* filename, char matrix[ROWS][COLS]) {
    int rows = 0, cols = 0; // Inicializar las variables para el número de filas y columnas
    int MAX_ROWS = 100,MAX_COLS = 100;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error al abrir el archivo.\n");
        return -1;
    }

    char line[MAX_COLS];
    while (fgets(line, sizeof(line), file) && rows < MAX_ROWS) {
        int col = 0;
        for (int i = 0; line[i] != '\0' && col < MAX_COLS; i++) {
            if (line[i] == '*' || line[i] == ' ' || line[i] == '/' || line[i] == 'x'){
                matrix[rows][col] = line[i];
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

