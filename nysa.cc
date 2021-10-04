// ****************************************
// dane z treści, obserwacje i przemyślenia
// ****************************************
// jest N sygnałów
// w tym M sygnałów wejściowych
// wypisujemy 2^M linii wyjścia
// więc M będzie małe (max 64 ale moim zdaniem nawet mniej (jakoś tak z 20-25)
// N może być duże (max 1e9 - 1 -> to jest wprost napisane w treści (realnie < 1e7))
// bramek jest co najwyżej tyle co sygnałów bo każda musi mieć osobne wyjście
// ****************************************

#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

// klasa reprezentująca typy bramek logicznych
enum class GateTypes {
    NOT, OR, NOR, AND, NAND, XOR
};

// todo: Gate jest z dużej a signal_t z małej -> chyba trzeba się zdecydować na jedną wersję
using Gate = tuple<GateTypes, int, vector<int>>; // <TYP, wyjście, wejścia>
using signal_t = uint32_t; // sygnały mają nr od 1 do 999999999 więc potrzeba conajmniej 32 bitowego typu

// przechowuje stan sygnałów w danej chwili
// potrzebna jest mapa, bo na kóncu wypisujemy posortowane
map<int, bool> signalStates;

// wektor przechowujący wszystkie bramki
// bramki indeksujemy od 0
vector<Gate> gates;

// dla każdego sygnału musimy pamiętać do jakich bramek wchodzi
// na wektorze trzymamy indeks bramki
// todo: trzeba to jakoś sensownie nazwać
unordered_map<signal_t, vector<int>> doJakichBramekIdzieSyngal;

// zbiór sygnałów wyjściowych
unordered_set<signal_t> outputs;

//todo:
// wczytując bramkę np. "AND 5 3 1"
// sprawdzasz czy wyjście nie ma zwarcia (czy nie ma go na secie outputs)
// wrzucasz wyjście na set
// wrzucasz bramkę na wektor gates
// każdego sygnału wejściowego dorzucasz indeks tej bramki do mapy doJakichBramekIdzieSyngal
// jak będzie błąd na wejściu to wychodzisz exit(0) więc funkcja może być voidem
// trzeba zmienić nazwę na coś po angielsku
void wczytaj() {

}

// todo: czy ta nazwa jest dobra?
bool checkSequentialHelper(size_t gateIdx, size_t visitedIdx,
                           vector<int> &visited) {
    if (visited[gateIdx] == visitedIdx) {
        return true;
    }
    for (int g : doJakichBramekIdzieSyngal[get<1>(gates[gateIdx])]) {
        checkSequentialHelper(g, visitedIdx, visited);
    }
    return false;
}

bool checkSequential() {
    vector<int> visited;
    for (size_t i = 0, j = 0; i < gates.size(); i++) {
        if (visited[i] != 0 && checkSequentialHelper(i, ++j, visited)) {
            return true;
        }
    }
    return false;
}

// liczy inputy (m z treści zadania)
// todo: zaimplementować to
int countInputs() {
    return 42;
}

// wypisywanie wyjśćia
void printSignalsCombination() {
    for (auto &el : signalStates) {
        cout << el.second << " ";
    }
    cout << endl;
}

int main() {
    wczytaj();
    if (checkSequential()) {
        cerr << "Error: sequential logic analysis has not yet been implemented.";
        return 0;
    }
    int m = countInputs();
    for (size_t i = 0; i < 1<<m; ++i) {
        //todo:
        // zamień i na binarkę (przemyśleć to)
        // podstaw pod signale
        // przetwórz układ
        printSignalsCombination();

    }
    return 0;
}
