#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm> // Para random_shuffle
#include <time.h>    // Para nanosleep
#include <fstream>   // Para manejo de archivos

using namespace std;

// Funcion para limpiar la pantalla
void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Funcion para imprimir el laberinto
void imprimirLaberinto(int** laberinto, int filas, int columnas, const vector<pair<int, int>>& camino) {
    int** laberintoConCamino = new int*[filas];
    for (int i = 0; i < filas; ++i) {
        laberintoConCamino[i] = new int[columnas];
        for (int j = 0; j < columnas; ++j) {
            laberintoConCamino[i][j] = laberinto[i][j];
        }
    }
    
    for (const auto& paso : camino) {
        laberintoConCamino[paso.first][paso.second] = 2; // Marcar el camino
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
        delete[] laberintoConCamino[i];
    }
    delete[] laberintoConCamino;
}

// Funcion auxiliar para realizar DFS y asegurar un camino valido
void dfs(int** laberinto, int filas, int columnas, int x, int y) {
    vector<pair<int, int>> movimientos = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    random_shuffle(movimientos.begin(), movimientos.end());

    for (const auto& mov : movimientos) {
        int nx = x + mov.first * 2;
        int ny = y + mov.second * 2;

        if (nx > 0 && nx < filas - 1 && ny > 0 && ny < columnas - 1 && laberinto[nx][ny] == 1) {
            laberinto[nx - mov.first][ny - mov.second] = 0;
            laberinto[nx][ny] = 0;
            dfs(laberinto, filas, columnas, nx, ny);
        }
    }
}

// Funcion para generar un laberinto aleatorio
int** generarLaberinto(int filas, int columnas) {
    if (filas < 3) filas = 3;
    if (columnas < 3) columnas = 3;

    int** laberinto = new int*[filas];
    for (int i = 0; i < filas; ++i) {
        laberinto[i] = new int[columnas];
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

// Funcion para guardar el laberinto en un archivo con un nombre dado
void guardarLaberintoEnArchivo(int** laberinto, int filas, int columnas, const char* nombreArchivo) {
    ofstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo para escribir el laberinto." << endl;
        return;
    }

    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            if (laberinto[i][j] == 1)
                archivo << "|"; // Paredes
            else
                archivo << " "; // Caminos
        }
        archivo << endl;
    }

    archivo.close();
}

// Funcion para leer un laberinto desde un archivo
int** leerLaberintoDesdeArchivo(const char* nombreArchivo, int& filas, int& columnas) {
    ifstream archivo(nombreArchivo);
    vector<string> lineas;
    string linea;

    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo para leer el laberinto." << endl;
        return nullptr;
    }

    while (getline(archivo, linea)) {
        lineas.push_back(linea);
    }

    archivo.close();

    filas = lineas.size();
    columnas = lineas[0].length();
    int** laberinto = new int*[filas];
    for (int i = 0; i < filas; ++i) {
        laberinto[i] = new int[columnas];
        for (int j = 0; j < columnas; ++j) {
            if (lineas[i][j] == ' ')
                laberinto[i][j] = 0; // Camino (0)
            else
                laberinto[i][j] = 1; // Pared (1)
        }
    }

    return laberinto;
}

// Funcion de backtracking para encontrar un camino desde la entrada a la salida
bool encontrarCamino(int** laberinto, int filas, int columnas, int x, int y, vector<pair<int, int>>& camino, int& totalPasos, int& pasosCorrectos) {
    if (x == filas - 2 && y == columnas - 1) { // Salida
        camino.push_back({x, y});
        pasosCorrectos++;
        return true;
    }

    vector<pair<int, int>> movimientos = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    random_shuffle(movimientos.begin(), movimientos.end());

    for (const auto& mov : movimientos) {
        int nx = x + mov.first;
        int ny = y + mov.second;

        if (nx >= 0 && nx < filas && ny >= 0 && ny < columnas && laberinto[nx][ny] == 0) {
            laberinto[nx][ny] = 2; // Marcar como parte del camino
            camino.push_back({nx, ny});

            totalPasos++; // Contar el paso solo si el movimiento es valido

            // Mostrar movimiento paso a paso
            limpiarPantalla();
            imprimirLaberinto(laberinto, filas, columnas, camino);
            printf("Total de pasos (incluyendo intentos erroneos): %d\n", totalPasos);
            printf("Total de pasos por el camino correcto: %d\n", pasosCorrectos);
            struct timespec req = {0, 500 * 1000000}; // 500 ms
            nanosleep(&req, nullptr);

            if (encontrarCamino(laberinto, filas, columnas, nx, ny, camino, totalPasos, pasosCorrectos)) {
                pasosCorrectos++; // Contar el paso correcto
                return true;
            }

            laberinto[nx][ny] = 0; // Desmarcar
            camino.pop_back();
        }
    }

    return false;
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
        vector<pair<int, int>> caminoVacio; // Vector vacio
        imprimirLaberinto(laberinto, filas, columnas, caminoVacio);

        // Guardar el laberinto en un archivo con el nombre especificado
        guardarLaberintoEnArchivo(laberinto, filas, columnas, nombreArchivo);

        for (int i = 0; i < filas; ++i) {
            delete[] laberinto[i];
        }
        delete[] laberinto;

    } else if (opcion == 2) {
        char nombreArchivo[100];
        printf("Ingrese el nombre del archivo que contiene el laberinto: ");
        scanf("%s", nombreArchivo);

        // Leer el laberinto desde el archivo
        int** laberintoLeido = leerLaberintoDesdeArchivo(nombreArchivo, filas, columnas);

        if (laberintoLeido == nullptr) {
            printf("El laberinto no se pudo leer o el archivo esta vacio.\n");
            return 1;
        }

        // Encontrar camino en el laberinto leído
        vector<pair<int, int>> camino;
        int totalPasos = 0;
        int pasosCorrectos = 0;
        if (encontrarCamino(laberintoLeido, filas, columnas, 1, 0, camino, totalPasos, pasosCorrectos)) {
            printf("\nSe ha encontrado un camino hacia la salida en el laberinto leido:\n");
            imprimirLaberinto(laberintoLeido, filas, columnas, camino);
            printf("Total de pasos (incluyendo intentos erroneos): %d\n", totalPasos);
            printf("Total de pasos por el camino correcto: %d\n", pasosCorrectos);
        } else {
            printf("\nNo se encontro un camino hacia la salida en el laberinto leido.\n");
        }

        for (int i = 0; i < filas; ++i) {
            delete[] laberintoLeido[i];
        }
        delete[] laberintoLeido;
    } else {
        printf("Opcion no valida.\n");
    }

    return 0;
}
