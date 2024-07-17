#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // Para nanosleep
#include <string.h> // Para manejo de cadenas

// Definiciones de función
void limpiarPantalla();
void imprimirLaberinto(int** laberinto, int filas, int columnas, int camino[][2], int caminoSize);
void dfs(int** laberinto, int filas, int columnas, int x, int y);
int** generarLaberinto(int filas, int columnas);
void guardarLaberintoEnArchivo(int** laberinto, int filas, int columnas, const char* nombreArchivo);
int** leerLaberintoDesdeArchivo(const char* nombreArchivo, int* filas, int* columnas);
int encontrarCamino(int** laberinto, int filas, int columnas, int x, int y, int camino[][2], int* caminoSize, int* totalPasos, int* pasosCorrectos);

void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void imprimirLaberinto(int** laberinto, int filas, int columnas, int camino[][2], int caminoSize) {
    int** laberintoConCamino = (int**)malloc(filas * sizeof(int*));
    for (int i = 0; i < filas; ++i) {
        laberintoConCamino[i] = (int*)malloc(columnas * sizeof(int));
        for (int j = 0; j < columnas; ++j) {
            laberintoConCamino[i][j] = laberinto[i][j];
        }
    }

    for (int i = 0; i < caminoSize; ++i) {
        int x = camino[i][0];
        int y = camino[i][1];
        laberintoConCamino[x][y] = 2; // Marcar el camino
    }

    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            if (laberintoConCamino[i][j] == 1)
                printf("|"); // Paredes
            else if (laberintoConCamino[i][j] == 2)
                printf("*"); // Camino encontrado
            else
                printf(" "); // Caminos
        }
        printf("\n");
    }

    for (int i = 0; i < filas; ++i) {
        free(laberintoConCamino[i]);
    }
    free(laberintoConCamino);
}

void dfs(int** laberinto, int filas, int columnas, int x, int y) {
    int movimientos[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int i, j, temp[2];

    for (i = 0; i < 4; ++i) {
        j = rand() % 4;
        temp[0] = movimientos[i][0];
        temp[1] = movimientos[i][1];
        movimientos[i][0] = movimientos[j][0];
        movimientos[i][1] = movimientos[j][1];
        movimientos[j][0] = temp[0];
        movimientos[j][1] = temp[1];
    }

    for (i = 0; i < 4; ++i) {
        int nx = x + movimientos[i][0] * 2;
        int ny = y + movimientos[i][1] * 2;

        if (nx > 0 && nx < filas - 1 && ny > 0 && ny < columnas - 1 && laberinto[nx][ny] == 1) {
            laberinto[nx - movimientos[i][0]][ny - movimientos[i][1]] = 0;
            laberinto[nx][ny] = 0;
            dfs(laberinto, filas, columnas, nx, ny);
        }
    }
}

int** generarLaberinto(int filas, int columnas) {
    if (filas < 3) filas = 3;
    if (columnas < 3) columnas = 3;

    int** laberinto = (int**)malloc(filas * sizeof(int*));
    for (int i = 0; i < filas; ++i) {
        laberinto[i] = (int*)malloc(columnas * sizeof(int));
        for (int j = 0; j < columnas; ++j) {
            laberinto[i][j] = 1;
        }
    }

    srand(time(0));

    laberinto[1][1] = 0;
    dfs(laberinto, filas, columnas, 1, 1);

    // Asegurar la entrada y la salida del laberinto
    laberinto[1][0] = 0; // Entrada
    laberinto[1][1] = 0; // Asegurar un camino inicial desde la entrada
    laberinto[filas - 2][columnas - 1] = 0; // Salida

    return laberinto;
}

void guardarLaberintoEnArchivo(int** laberinto, int filas, int columnas, const char* nombreArchivo) {
    FILE* archivo = fopen(nombreArchivo, "w");

    if (archivo == NULL) {
        fprintf(stderr, "Error al abrir el archivo para escribir el laberinto.\n");
        return;
    }

    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            if (laberinto[i][j] == 1)
                fputc('|', archivo); // Paredes
            else
                fputc(' ', archivo); // Caminos
        }
        fputc('\n', archivo);
    }

    fclose(archivo);
}

int** leerLaberintoDesdeArchivo(const char* nombreArchivo, int* filas, int* columnas) {
    FILE* archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        fprintf(stderr, "Error al abrir el archivo para leer el laberinto.\n");
        return NULL;
    }

    char linea[256];
    char* lineas[256];
    int lineasCount = 0;

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        lineas[lineasCount] = strdup(linea);
        lineasCount++;
    }

    fclose(archivo);

    *filas = lineasCount;
    *columnas = strlen(lineas[0]) - 1; // Excluir el salto de línea

    int** laberinto = (int**)malloc(*filas * sizeof(int*));
    for (int i = 0; i < *filas; ++i) {
        laberinto[i] = (int*)malloc(*columnas * sizeof(int));
        for (int j = 0; j < *columnas; ++j) {
            if (lineas[i][j] == ' ')
                laberinto[i][j] = 0; // Camino (0)
            else
                laberinto[i][j] = 1; // Pared (1)
        }
        free(lineas[i]);
    }

    return laberinto;
}

int encontrarCamino(int** laberinto, int filas, int columnas, int x, int y, int camino[][2], int* caminoSize, int* totalPasos, int* pasosCorrectos) {
    if (x == filas - 2 && y == columnas - 1) { // Salida
        camino[*caminoSize][0] = x;
        camino[*caminoSize][1] = y;
        (*caminoSize)++;
        (*pasosCorrectos)++;
        return 1;
    }

    int movimientos[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int i, j, temp[2];

    for (i = 0; i < 4; ++i) {
        j = rand() % 4;
        temp[0] = movimientos[i][0];
        temp[1] = movimientos[i][1];
        movimientos[i][0] = movimientos[j][0];
        movimientos[i][1] = movimientos[j][1];
        movimientos[j][0] = temp[0];
        movimientos[j][1] = temp[1];
    }

    for (i = 0; i < 4; ++i) {
        int nx = x + movimientos[i][0];
        int ny = y + movimientos[i][1];

        if (nx >= 0 && nx < filas && ny >= 0 && ny < columnas && laberinto[nx][ny] == 0) {
            laberinto[nx][ny] = 2; // Marcar como parte del camino
            camino[*caminoSize][0] = nx;
            camino[*caminoSize][1] = ny;
            (*caminoSize)++;

            (*totalPasos)++; // Contar el paso solo si el movimiento es válido

            // Mostrar movimiento paso a paso
            limpiarPantalla();
            imprimirLaberinto(laberinto, filas, columnas, camino, *caminoSize);
            printf("Total de pasos (incluyendo intentos erróneos): %d\n", *totalPasos);
            printf("Total de pasos por el camino correcto: %d\n", *pasosCorrectos);
            struct timespec req = {0, 500 * 1000000}; // 500 ms
            nanosleep(&req, NULL);

            if (encontrarCamino(laberinto, filas, columnas, nx, ny, camino, caminoSize, totalPasos, pasosCorrectos)) {
                (*pasosCorrectos)++; // Contar el paso correcto
                return 1;
            }

            laberinto[nx][ny] = 0; // Desmarcar
            (*caminoSize)--;
        }
    }

    return 0;
}

int main() {
    int filas, columnas;
    int opcion;

    printf("Seleccione una opcion:\n");
    printf("1. Generar un laberinto aleatorio y guardarlo en un archivo\n");
    printf("2. Resolver un laberinto desde un archivo\n");
    scanf("%d", &opcion);

    if (opcion == 1) {
        printf("Ingrese el numero de filas: ");
        scanf("%d", &filas);
        printf("Ingrese el numero de columnas: ");
        scanf("%d", &columnas);

        if (filas < 3 || columnas < 3) {
            printf("El numero de filas y columnas debe ser al menos 3 para crear un laberinto valido.\n");
            return 1;
        }

        char nombreArchivo[100];
        printf("Ingrese el nombre del archivo donde desea guardar el laberinto: ");
        scanf("%s", nombreArchivo);

        int** laberinto = generarLaberinto(filas, columnas);
        printf("Laberinto generado:\n");
        int caminoVacio[1][2]; // Vector vacío
        imprimirLaberinto(laberinto, filas, columnas, caminoVacio, 0);

        // Guardar el laberinto en un archivo con el nombre especificado
        guardarLaberintoEnArchivo(laberinto, filas, columnas, nombreArchivo);

        for (int i = 0; i < filas; ++i) {
            free(laberinto[i]);
        }
        free(laberinto);

    } else if (opcion == 2) {
        char nombreArchivo[100];
        printf("Ingrese el nombre del archivo que contiene el laberinto: ");
        scanf("%s", nombreArchivo);

        // Leer el laberinto desde el archivo
        int** laberintoLeido = leerLaberintoDesdeArchivo(nombreArchivo, &filas, &columnas);

        if (laberintoLeido == NULL) {
            printf("El laberinto no se pudo leer o el archivo esta vacio.\n");
            return 1;
        }

        // Encontrar camino en el laberinto leído
        int camino[filas * columnas][2];
        int caminoSize = 0;
        int totalPasos = 0;
        int pasosCorrectos = 0;
        if (encontrarCamino(laberintoLeido, filas, columnas, 1, 0, camino, &caminoSize, &totalPasos, &pasosCorrectos)) {
            printf("\nSe ha encontrado un camino hacia la salida en el laberinto leido:\n");
            imprimirLaberinto(laberintoLeido, filas, columnas, camino, caminoSize);
            printf("Total de pasos (incluyendo intentos erroneos): %d\n", totalPasos);
            printf("Total de pasos por el camino correcto: %d\n", pasosCorrectos);
        } else {
            printf("\nNo se encontro un camino hacia la salida en el laberinto leido.\n");
        }

        for (int i = 0; i < filas; ++i) {
            free(laberintoLeido[i]);
        }
        free(laberintoLeido);
    } else {
        printf("Opcion no valida.\n");
    }

    return 0;
}