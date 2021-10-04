#include <iostream>
#include <unordered_map>
#include <map>
#include <vector>

using namespace std;

enum class GateTypes {
    NOT, OR, NOR, AND, NAND, XOR
};

// BĘDĄ NAJWYŻEJ 64 WEJŚCIA

using Gate = tuple<GateTypes, int, vector<int>>; // TYP, wyjście, wejścia
using signal_t = uint32_t;

map<int, bool> inputStates; // przechowuje stan inputu w danej chwili, intputy na kóncu wypisujemy posortowane
unordered_map<signal_t, vector<int>> doJakichBramekIdzieSyngal;
vector<Gate> gates;
vector<int> visited; // to brzmi jak zmienna lokalna

// to się zaklepie na końcu
void wczytaj() {

}

bool checkSequentialHelper(size_t gateIdx, size_t visitedIdx) {
    if (visited[gateIdx] == visitedIdx) {
        return true;
    }
    for (int g : doJakichBramekIdzieSyngal[get<1>(gates[gateIdx])]) {
        checkSequentialHelper(g, visitedIdx);
    }
    return false;
}

bool checkSequential() {
    for (size_t i = 0, j = 0; i < gates.size(); i++) {
        if (visited[i] != 0 && checkSequentialHelper(i, ++j)) {
            return true;
        }
    }
    return false;
}

// liczy inputy (m z treści zadania)
int countInputs() {
    return 42;
}

void printSignalsCombination() {
    for (auto &el : inputStates) {
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
        // zamień i na binarkę
        // podstaw pod signale
        // przetwórz układ
        printSignalsCombination();

    }
    return 0;
}
