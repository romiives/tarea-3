#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

//nodo lista adyacencia
struct NodoLista {
    int destino;
    NodoLista* sig;
};

//lista adyacencia
struct ListaAdy {
    NodoLista* inicio;
};

//clase grafo
class Grafo {
private:
    int n; 
    ListaAdy* ady; 

public:
    Grafo(int nodos) {
        n = nodos;
        ady = new ListaAdy[n + 1]; 
        for (int i = 1; i <= n; i++) {
            ady[i].inicio = nullptr;
        }
    }

    void agregarArco(int origen, int destino) {
        NodoLista* nuevo = new NodoLista{ destino, ady[origen].inicio };
        ady[origen].inicio = nuevo;
    }

    void imprimirGrafo() {
        for (int i = 1; i <= n; i++) {
            cout<< i << ": ";
            NodoLista* actual = ady[i].inicio;
            while (actual != nullptr) {
                cout << actual->destino << " ";
                actual = actual->sig;
            }
            cout<< endl;
        }
    }

    ListaAdy* getAdy() { return ady; }

    int getNodos() { return n; }

    ~Grafo() {
        for (int i = 1; i <= n; i++) {
            NodoLista* actual = ady[i].inicio;
            while (actual != nullptr) {
                NodoLista* temp = actual;
                actual = actual->sig;
                delete temp;
            }
        }
        delete[] ady;
    }
};

//funcion bfs que sera usada para solicitar los conductores
void bfs(ListaAdy* ady, int N, int inicio, int* padre, int* distancia) {
    bool* visitado = new bool[N + 1];
    for (int i = 1; i <= N; i++) {
        visitado[i] = false;
        distancia[i] = -1;
        padre[i] = -1;
    }
    int* cola = new int[N + 1];
    int frente = 0, fin = 0;
    cola[fin++] = inicio;
    visitado[inicio] = true;
    distancia[inicio] = 0;
    while (frente < fin) {
        int actual = cola[frente++];
        NodoLista* vecino = ady[actual].inicio;
        while (vecino != nullptr) {
            int v = vecino->destino;
            if (!visitado[v]) {
                visitado[v] = true;
                distancia[v] = distancia[actual] + 1;
                padre[v] = actual;
                cola[fin++] = v;
            }
            vecino = vecino->sig;
        }
    }
    delete[] cola;
    delete[] visitado;
}
void reconstruirRuta(int* padre, int destino, int* ruta, int& largo) {
    largo = 0;
    int actual = destino;
    while (actual != -1) {
        ruta[largo++] = actual;
        actual = padre[actual];
    }
    //invertir la ruta
    for (int i = 0; i < largo / 2; i++) {
        int temp = ruta[i];
        ruta[i] = ruta[largo - 1 - i];
        ruta[largo - 1 - i] = temp;
    }
}

//funcion para solicitar un uber
void solicitarUber(Grafo& ciudad, int* conductores, int cantConductores, int inicio, int destino) {
    int N = ciudad.getNodos();
    ListaAdy* ady = ciudad.getAdy();
    int mejorConductor = -1;
    int mejorDist = N + 1;
    int* padre = new int[N + 1];
    int* dist = new int[N + 1];
    //aca busca al conductor mas cercano
    for (int i = 0; i < cantConductores; i++) {
        int pos = conductores[i];
        bfs(ady, N, pos, padre, dist);
        if (dist[inicio] != -1 && dist[inicio] < mejorDist) {
            mejorDist = dist[inicio];
            mejorConductor = i;
        }
    }
    if (mejorConductor == -1) {
        cout << "Ruta: {} - Costo: -1" << endl;
        delete[] padre;
        delete[] dist;
        return;
    }
    int conductorPos = conductores[mejorConductor];
    //ruta conductor a pasajero
    bfs(ady, N, conductorPos, padre, dist);
    int* ruta1 = new int[N + 1];
    int largo1;
    reconstruirRuta(padre, inicio, ruta1, largo1);
    //ruta pasajero a su destino
    bfs(ady, N, inicio, padre, dist);
    if (dist[destino] == -1) {
        cout << "Ruta: {} - Costo: -1" << endl;
        delete[] padre; delete[] dist;
        delete[] ruta1;
        return;
    }
    int* ruta2 = new int[N + 1];
    int largo2;
    reconstruirRuta(padre, destino, ruta2, largo2);
    //imprimir la ruta combinada
    cout<< "Ruta: {";
    for (int i = 0; i < largo1 - 1; i++)
        cout<< ruta1[i] << ", ";
    for (int i = 0; i < largo2; i++) {
        cout<< ruta2[i];
        if (i != largo2 - 1) cout << ", ";
    }
    cout<< "}";
    //costo
    int costo = 300 * (largo1 - 1) + 500 * (largo2 - 1);
    cout<< " - Costo: " << costo << endl;
    //actualizacion de la posición del conductor
    conductores[mejorConductor]=destino;
    delete[] padre;
    delete[] dist;
    delete[] ruta1;
    delete[] ruta2;
}

//programa principal
int main() {
    ifstream archivo("data2.txt");
    if (!archivo) {
        cout<< "Error: No se pudo abrir data2.txt" <<endl;
        return 1;
    }

    int N, A, C; //nodos, arcos, conductores
    archivo >> N >> A >> C;
    Grafo ciudad(N);
    //leer arcos
    for (int i = 0; i < A; i++) {
        int u, v;
        archivo>> u >> v;
        ciudad.agregarArco(u, v);
    }
    //leer conductores
    int* conductores = new int[C];
    for (int i = 0; i < C; i++) {
        archivo>> conductores[i];
    }
    archivo.close();
    //imprimir grafo
    cout<< "Grafo cargado desde data2.txt:\n";
    ciudad.imprimirGrafo();
    //imprimir conductores
    cout<< "\nConductores ubicados en los nodos: ";
    for (int i = 0; i < C; i++) {
        cout<< conductores[i]<<" ";
    }
    cout<< endl;
    delete[] conductores;
    return 0;
}