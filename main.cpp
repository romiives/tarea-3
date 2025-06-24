#include <iostream>
#include <fstream>
using namespace std;

struct Nodo {
    int destino;
    Nodo* sig;
};

// TDA grafo
class Grafo {
private:
    Nodo** ady;
    int V;
public:
    Grafo(int vertices) {
        V = vertices;
        ady = new Nodo*[V+1];
        for (int i = 1; i <= V; ++i) {
            ady[i] = NULL;
        }
    }

    // agregamos un arco dirigido desde el origen al destino
    void agregarArco(int origen, int destino) {
        Nodo* nuevo = new Nodo{destino, ady[origen]};
        ady[origen] = nuevo;
    }

    Nodo* obtenerAdyacentes(int v) {
        return ady[v];
    }

    ~Grafo() {
        for (int i = 1; i <= V; ++i) {
            Nodo* actual = ady[i];
            while (actual) {
                Nodo* temp = actual;
                actual = actual->sig;
                delete temp;
            }
        }
        delete[] ady;
    }
};

//recorremos con BFS
void bfs(Grafo& g, int origen, int* dist, int* prev, int V) {
    bool* visitado = new bool[V+1];
    for (int i = 1; i <= V; ++i) {
        visitado[i] = false;
        dist[i] = -1;
        prev[i] = -1;
    }
    int* cola = new int[V];
    int frente = 0, fin = 0;
    visitado[origen] = true;
    dist[origen] = 0;
    cola[fin++] = origen;

    while (frente < fin) {
        int u = cola[frente++];
        Nodo* temp = g.obtenerAdyacentes(u);
        while (temp) {
            int v = temp->destino;
            if (!visitado[v]) {
                visitado[v] = true;
                dist[v] = dist[u] + 1;
                prev[v] = u;
                cola[fin++] = v;
            }
            temp = temp->sig;
        }
    }
    delete[] cola;
    delete[] visitado;
}

//usando el arreglo prev recontruimos el camino 
void reconstruirCamino(int destino, int* prev, int* camino, int& largo) {
    int temp[100];
    int n = 0;
    while (destino != -1) {
        temp[n++] = destino;
        destino = prev[destino];
    }
    largo = n;
    for (int i = 0; i < n; ++i) {
        camino[i] = temp[n - 1 - i];
    }
}

//se imprime el camino 
void imprimirRuta(int* camino, int largo) {
    cout << "{";
    for (int i = 0; i < largo; ++i) {
        cout << camino[i];
        if (i < largo - 1) cout << ", ";
    }
    cout << "}";
}

//esta funcion encuentra al conductor mas cercano y calcula la ruta completa con el costo
void solicitarUber(Grafo& g, int* conductores, int C, int inicio, int destino) {
    int dist[100], prev[100];
    int camino1[100], camino2[100];
    int mejorDist = 1e9, mejorConductor = -1;
    int V = 100;

    for (int i = 0; i < C; ++i) {
        bfs(g, conductores[i], dist, prev, V);
        if (dist[inicio] != -1 && dist[inicio] < mejorDist) {
            mejorDist = dist[inicio];
            mejorConductor = i;
        }
    }

    if (mejorConductor == -1) {
        cout << "Ruta: {} - Costo: -1" << endl;
        return;
    }

    int largo1 = 0, largo2 = 0;
    bfs(g, conductores[mejorConductor], dist, prev, V);
    reconstruirCamino(inicio, prev, camino1, largo1);

    bfs(g, inicio, dist, prev, V);
    if (dist[destino] == -1) {
        cout << "Ruta: {} - Costo: -1" << endl;
        return;
    }
    reconstruirCamino(destino, prev, camino2, largo2);

    int costo = 300 * (largo1 - 1) + 500 * (largo2 - 1);
    cout << "Ruta: ";
    if (largo1 > 1) {
        imprimirRuta(camino1, largo1);
        cout << " + ";
    }
    imprimirRuta(camino2, largo2);
    cout << " - Costo: " << costo << endl;

    // el conductor llega al destino
    conductores[mejorConductor] = destino;
}

int main() {
    ifstream archivo("data1.txt");
    int V, E, C;
    archivo >> V >> E >> C;
    Grafo ciudad(V);

    // Arcos
    for (int i = 0; i < E; ++i) {
        int a, b;
        archivo >> a >> b;
        ciudad.agregarArco(a, b);
    }

    int* conductores = new int[C];
    for (int i = 0; i < C; ++i) {
        archivo >> conductores[i];
    }
    archivo.close();

    int origen, destino;
    cout << "Ingrese viajes en formato 'origen destino' (termina con -1 -1):" << endl;
    while (true) {
        cout << ">> ";
        cin >> origen >> destino;
        if (origen == -1 && destino == -1) break;
        solicitarUber(ciudad, conductores, C, origen, destino);
    }

    // Liberamos memoria
    delete[] conductores;
    return 0;
}
