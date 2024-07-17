#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm> // Para random_shuffle
#include <time.h>    // Para nanosleep
#include <fstream>   // Para manejo de archivos

using namespace std;

// Función para limpiar la pantalla
void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Función para imprimir el laberinto
void imprimirLaberinto(const vector<vector<int>>& laberinto, const vector<pair<int, int>>& camino) {
    vector<vector<int>> laberintoConCamino = laberinto;
    for (const auto& paso : camino) {
        laberintoConCamino[paso.first][paso.second] = 2; // Marcar el camino
    }

    for (const auto& fila : laberintoConCamino) {
        for (const auto& celda : fila) {
            if (celda == 1)
                cout << "|"; // Paredes
            else if (celda == 2)
                cout << "*"; // Camino encontrado
            else
                cout << " "; // Caminos
        }
        cout << endl;
    }
}

// Función auxiliar para realizar DFS y asegurar un camino válido
void dfs(vector<vector<int>>& laberinto, int x, int y) {
    vector<pair<int, int>> movimientos = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    random_shuffle(movimientos.begin(), movimientos.end());

    for (const auto& mov : movimientos) {
        int nx = x + mov.first * 2;
        int ny = y + mov.second * 2;

        if (nx > 0 && nx < static_cast<int>(laberinto.size() - 1) && ny > 0 && ny < static_cast<int>(laberinto[0].size() - 1) && laberinto[nx][ny] == 1) {
            laberinto[nx - mov.first][ny - mov.second] = 0;
            laberinto[nx][ny] = 0;
            dfs(laberinto, nx, ny);
        }
    }
}

// Función para generar un laberinto aleatorio
vector<vector<int>> generarLaberinto(int filas, int columnas) {
    if (filas < 3) filas = 3;
    if (columnas < 3) columnas = 3;

    vector<vector<int>> laberinto(filas, vector<int>(columnas, 1));

    srand(time(0));

    laberinto[1][1] = 0;
    dfs(laberinto, 1, 1);
    
    // Asegurar la entrada y la salida del laberinto
    laberinto[1][0] = 0; // Entrada
    laberinto[1][1] = 0; // Asegurar un camino inicial desde la entrada
    laberinto[filas - 2][columnas - 1] = 0; // Salida

    return laberinto;
}

// Función para guardar el laberinto en un archivo con un nombre dado
void guardarLaberintoEnArchivo(const vector<vector<int>>& laberinto, const string& nombreArchivo) {
    ofstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo para escribir el laberinto." << endl;
        return;
    }

    for (const auto& fila : laberinto) {
        for (const auto& celda : fila) {
            if (celda == 1)
                archivo << "|"; // Paredes
            else
                archivo << " "; // Caminos
        }
        archivo << endl;
    }

    archivo.close();
}

// Función para leer un laberinto desde un archivo
vector<vector<int>> leerLaberintoDesdeArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    vector<vector<int>> laberinto;
    string linea;

    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo para leer el laberinto." << endl;
        return laberinto;
    }

    while (getline(archivo, linea)) {
        vector<int> fila;
        for (char celda : linea) {
            if (celda == ' ')
                fila.push_back(0); // Camino (0)
            else
                fila.push_back(1); // Pared (1)
        }
        laberinto.push_back(fila);
    }

    archivo.close();
    return laberinto;
}

// Función de backtracking para encontrar un camino desde la entrada a la salida
bool encontrarCamino(vector<vector<int>>& laberinto, int x, int y, vector<pair<int, int>>& camino, int& totalPasos, int& pasosCorrectos) {
    int filas = static_cast<int>(laberinto.size());
    int columnas = static_cast<int>(laberinto[0].size());

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

            totalPasos++; // Contar el paso solo si el movimiento es válido

            // Mostrar movimiento paso a paso
            limpiarPantalla();
            imprimirLaberinto(laberinto, camino);
            cout << "Total de pasos (incluyendo intentos erroneos): " << totalPasos << endl;
            cout << "Total de pasos por el camino correcto: " << pasosCorrectos << endl;
            struct timespec req = {0, 500 * 1000000}; // 500 ms
            nanosleep(&req, nullptr);

            if (encontrarCamino(laberinto, nx, ny, camino, totalPasos, pasosCorrectos)) {
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

    cout << "Seleccione una opcion:" << endl;
    cout << "1. Generar un laberinto aleatorio y guardarlo en un archivo" << endl;
    cout << "2. Resolver un laberinto desde un archivo" << endl;
    cin >> opcion;

    if (opcion == 1) {
        cout << "Ingrese el numero de filas: ";
        cin >> filas;
        cout << "Ingrese el numero de columnas: ";
        cin >> columnas;

        if (filas < 3 || columnas < 3) {
            cout << "El numero de filas y columnas debe ser al menos 3 para crear un laberinto valido." << endl;
            return 1;
        }

        string nombreArchivo;
        cout << "Ingrese el nombre del archivo donde desea guardar el laberinto: ";
        cin >> nombreArchivo;

        vector<vector<int>> laberinto = generarLaberinto(filas, columnas);
        cout << "Laberinto generado:\n";
        imprimirLaberinto(laberinto, {});

        // Guardar el laberinto en un archivo con el nombre especificado
        guardarLaberintoEnArchivo(laberinto, nombreArchivo);

    } else if (opcion == 2) {
        string nombreArchivo;
        cout << "Ingrese el nombre del archivo que contiene el laberinto: ";
        cin >> nombreArchivo;

        // Leer el laberinto desde el archivo
        vector<vector<int>> laberintoLeido = leerLaberintoDesdeArchivo(nombreArchivo);

        if (laberintoLeido.empty()) {
            cout << "El laberinto no se pudo leer o el archivo esta vacio." << endl;
            return 1;
        }

        // Encontrar camino en el laberinto leído
        vector<pair<int, int>> camino;
        int totalPasos = 0;
        int pasosCorrectos = 0;
        if (encontrarCamino(laberintoLeido, 1, 0, camino, totalPasos, pasosCorrectos)) {
            cout << "\nSe ha encontrado un camino hacia la salida en el laberinto leido:\n";
            imprimirLaberinto(laberintoLeido, camino);
            cout << "Total de pasos (incluyendo intentos erroneos): " << totalPasos << endl;
            cout << "Total de pasos por el camino correcto: " << pasosCorrectos << endl;
        } else {
            cout << "\nNo se encontro un camino hacia la salida en el laberinto leido.\n";
        }
    } else {
        cout << "Opcion no valida." << endl;
    }

    return 0;
}