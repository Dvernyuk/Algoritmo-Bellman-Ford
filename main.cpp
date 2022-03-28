// Dmytro Vernyuk
// Grafos | Practica 8: Bellman-Ford

#include <iostream>
#include <fstream>
#include <queue> 
#include <vector>
#include <time.h>
#include <chrono>
using namespace std;

int N_VERTICES = 5; // Numero de vertices del grafo
const int MAX_VERTICES = 5000; // Maximo de vertices que puede tener el grafo

typedef struct arista {
    int origen;
    int destino;
    int coste;
} t_arista;

ostream& operator<<(ostream& os, const t_arista& a) {
    os << "arista " << a.origen << " -> " << a.destino << " de coste " << a.coste;
    return os;
}

// Añadir arista
void addArista(vector<t_arista> grafo[], int o, int d, int c) {
    t_arista arista = { o,d,c };
    grafo[o].push_back(arista);
    if(o != d) // Si el origen y el destino son distintos la añadimos tanto en la lista del origen como la del destino
        grafo[d].push_back(arista);
}

void crearGrafo(vector<t_arista> grafo[]) {
    int destino, coste, cara_cruz;
    bool end ; // Variable que permite o no la creacion de aristas
    bool asignados[MAX_VERTICES];
    queue<int> aux;

    for (int i = 0; i < N_VERTICES; i++)
        asignados[i] = false;

    srand((int)time(NULL));

    for (int i = 0; i < N_VERTICES; i++) {
        end = false;

        while (aux.size() < N_VERTICES && !end) {
            cara_cruz = rand() % 2;
            if (cara_cruz == 1 || grafo[i].empty()) { // La primera arista siempre se pone
               
                if (grafo[i].empty()) { // Como minimo habra una arista no recursiva en todos los nodos, sea el origen o el destino
                    do { destino = rand() % N_VERTICES; } while (destino == i);
                }
                else { // Solo habra una arista con el mismo origen y direccion
                    do { destino = rand() % N_VERTICES; } while (asignados[destino] == true);
                }
                asignados[destino] = true;
                aux.push(destino);

                coste = rand() % 10;
                cara_cruz = rand() % 2;
                if (cara_cruz == 0) { // "¿Es negativo el coste?"
                    coste *= -1;
                }
                cara_cruz = 1;

                addArista(grafo, i, destino, coste);
            }
            else {
                end = true;
            }
        }

        while (!aux.empty()) {
            asignados[aux.front()] = false;
            aux.pop();
        }
    }
}

void eliminarGrafo(vector<t_arista> grafo[]) {
    for (int i = 0; i < N_VERTICES; i++) {
        grafo[i].clear();
    }
}

void printGrafo(const vector<t_arista> grafo[]) {
    for (int i = 0; i < N_VERTICES; ++i) {
        cout << "Vertice " << i << ":" << endl;
        for (const t_arista& arista : grafo[i])
            if (arista.origen == i)
                cout << "\t" << arista << endl;
        cout << endl;
    }
}

void printDist(const float dist[]) {
    cout << "Distancias desde el vertice 0:" << endl;
    for (int i = 0; i < N_VERTICES; i++) {
        cout << "\tvertice " << i << " -> " << dist[i] << endl;
    }
    cout << endl;
}

double bellmanFord(vector<t_arista> grafo[], float dist[]) {
    int end;

    dist[0] = 0;
    for (int i = 1; i < N_VERTICES; i++)
        dist[i] = INFINITY;

    // Inicializacion para el calculo del tiempo
    using clock = std::chrono::system_clock;
    using sec = std::chrono::duration<double, std::milli>;
    const auto t_inicio = clock::now();

    // Primera iteracion para las distancias
    end = false;
    for (int i = 0; i < N_VERTICES - 1 && !end; i++) {
        end = true;
        for (int j = 0; j < N_VERTICES; j++)
            for (t_arista a : grafo[j])
                if (dist[a.origen] + a.coste < dist[a.destino]) {
                    dist[a.destino] = dist[a.origen] + a.coste;
                    end = false;
                }
    }

    // Segunda iteracion para los bucles negativos
    end = false;
    for (int i = 0; i < N_VERTICES - 1 && !end; i++) {
        end = true;
        for (int j = 0; j < N_VERTICES; j++)
            for (t_arista a : grafo[j])
                if (dist[a.origen] + a.coste < dist[a.destino]) {
                    dist[a.destino] = -INFINITY;
                    end = false;
                }
    }

    const sec t_final = clock::now() - t_inicio; // Calculo del tiempo transcurrido

    return t_final.count();
}

void tiempos(vector<t_arista> grafo[], float dist[]) {
    int nv = N_VERTICES;
    double aux = 0, tot;
    int j;

    ofstream out("tiempos.txt");

    for (N_VERTICES = 500; N_VERTICES <= MAX_VERTICES; N_VERTICES += 3) { // Sumamos los vertices de tres en tres (lento)
        tot = 0;
        j = 0;
        while (j < 3) { // Buscamos las tres primeras soluciones que sean mayores o igual a 10 para despues hacer la media
            eliminarGrafo(grafo);
            crearGrafo(grafo);

            aux = bellmanFord(grafo, dist);
            if ((int)aux >= 10) {
                tot += aux;
                j++;
            }
        }
        tot /= j;

        out << N_VERTICES << " " << tot << endl;
    }

    out.close();

    N_VERTICES = nv;
}

// El menu de la aplicacion
int main() {
    bool menu = true;
    int opt;
    float dist[MAX_VERTICES];
    vector<t_arista> grafo[MAX_VERTICES];

    cout << "/// N_VERTICES = " << N_VERTICES << " ///" << endl;
    while (menu) {
        cout << "1 ~> Hallar distancias minimas" << endl;
        cout << "2 ~> Crear grafo" << endl;
        cout << "3 ~> Ver dintancias" << endl;
        cout << "4 ~> Ver grafo" << endl;
        cout << "8 ~> Generar tiempos.txt (para el grafico)" << endl;
        cout << "9 ~> Cambiar N_VERTICES" << endl;
        cout << "0 ~> Salir" << endl;
        cin >> opt;
        cout << endl;

        switch (opt) {
            case 1:
                cout << "Tiempo total " << bellmanFord(grafo, dist) << " ms" << endl;
                if (N_VERTICES <= 10)
                    printDist(dist);
                break;
            case 2:
                eliminarGrafo(grafo);
                crearGrafo(grafo);
                if (N_VERTICES <= 10)
                    printGrafo(grafo);
                break;
            case 3:
                printDist(dist);
                break;
            case 4:
                printGrafo(grafo);
                break;
            case 8:
                tiempos(grafo, dist);
                break;
            case 9:
                int x;
                cout << "2 <= x <= 5000" << endl << "x = ";
                cin >> x;
                if (x > 1 && x <= 5000)
                    N_VERTICES = x;
                cout << endl << "/// N_VERTICES = " << N_VERTICES << " ///" << endl;
                break;
            case 0:
                menu = false;
        }
    }

    return 0;
}