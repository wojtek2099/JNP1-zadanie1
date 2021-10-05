// ****************************************
// dane z treści, obserwacje i przemyślenia
// ****************************************
// jest N sygnałów
// w tym M sygnałów wejściowych
// wypisujemy 2^M linii wyjścia
// więc M będzie małe (max 64 ale moim zdaniem nawet mniej (jakoś tak z 20-25)
// N może być duże (max 1e9 - 1 -> to jest wprost napisane w treści (realnie < 1e7))
// bramek jest co najwyżej tyle co sygnałów bo każda musi mieć osobne wyjście
// układ bramek towrzy DAG (directed acyclic graph)
// ****************************************

#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>
#include <stack>

using namespace std;

// klasa reprezentująca typy bramek logicznych
enum class GateTypes {
    NOT, OR, NOR, AND, NAND, XOR
};

// todo: Gate jest z dużej a signal_t z małej -> chyba trzeba się zdecydować na jedną wersję
//  czy zdefiniować też typ dla bramek? (using gate_t = signal_t;)
using signal_t = uint32_t; // sygnały mają nr od 1 do 999999999 więc potrzeba conajmniej 32 bitowego typu
using Gate = tuple<GateTypes, signal_t, vector<signal_t>>; // <TYP, wyjście, wejścia>

// przechowuje stan sygnałów w danej chwili
// potrzebna jest mapa, bo na kóncu wypisujemy posortowane
map<signal_t, bool> signalStates;

// wektor przechowujący wszystkie bramki
// bramki indeksujemy od 0
vector<Gate> gates;

// bramki posortowane topologicznie
vector<int> topologicalOrder;

// dla każdego sygnału musimy pamiętać do jakich bramek wchodzi
// na wektorze trzymamy indeks bramki
// todo: trzeba to jakoś sensownie nazwać
unordered_map<signal_t, vector<int>> doJakichBramekIdzieSyngal;

// zbiór sygnałów wyjściowych
set<signal_t> inputs;

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
//  czy to 1 i 2 jest ok czy to magiczne stałe? (może enum zrobić?)
bool topologicalSortHelper(int gateIdx, vector<int8_t> &visited, stack<int> &stos) {
    if (visited[gateIdx] == 1) {
        return false;
    }
    visited[gateIdx] = 1;
    for (int g : doJakichBramekIdzieSyngal[get<1>(gates[gateIdx])]) {
        if (visited[g] == 0 && !topologicalSortHelper(g, visited, stos)) {
            return false;
        }
    }
    visited[gateIdx] = 2;
    stos.push(gateIdx);
    return true;
}

bool topologicalSort() {
    // todo: może tu zwykły int?
    vector<int8_t> visited;
    // todo: nazwać to
    stack<int> stos;
    for (int i = 0; (size_t) i < gates.size(); i++) {
        if (visited[i] == 0 && !topologicalSortHelper(i, visited, stos)) {
            return false;
        }
    }
    while (!stos.empty()) {
        topologicalOrder.push_back(stos.top());
        stos.pop();
    }
    return true;
}

void findInputs() {
    for (auto &signal : signalStates) {
        if (!outputs.count(signal.first)) {
            inputs.insert(signal.first);
        }
    }
}

bool evalOr(vector<signal_t> &signals) {
    for (signal_t signal : signals) {
        if (signalStates[signal]) {
            return true;
        }
    }
    return false;
}

bool evalAnd(vector<signal_t> &signals) {
    for (signal_t signal : signals) {
        if (!signalStates[signal]) {
            return false;
        }
    }
    return true;
}

// todo: może by tu coś do zmiennych pomocniczych wyciągnął?
void eval() {
    for (int gateIdx : topologicalOrder) {
        switch (get<0>(gates[gateIdx])) {
            case GateTypes::NOT:
                signalStates[get<1>(gates[gateIdx])] = !get<2>(gates[gateIdx])[0];
                break;
            case GateTypes::OR:
                signalStates[get<1>(gates[gateIdx])] = evalOr(get<2>(gates[gateIdx]));
                break;
            case GateTypes::NOR:
                signalStates[get<1>(gates[gateIdx])] = !evalOr(get<2>(gates[gateIdx]));
                break;
            case GateTypes::AND:
                signalStates[get<1>(gates[gateIdx])] = evalAnd(get<2>(gates[gateIdx]));
                break;
            case GateTypes::NAND:
                signalStates[get<1>(gates[gateIdx])] = !evalAnd(get<2>(gates[gateIdx]));
                break;
            case GateTypes::XOR:
                signalStates[get<1>(gates[gateIdx])] = get<2>(gates[gateIdx])[0] ^ get<2>(gates[gateIdx])[0];
                break;
        }
    }
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
    if (!topologicalSort()) {
        cerr << "Error: sequential logic analysis has not yet been implemented.";
        return 0;
    }
    findInputs();
    size_t m = inputs.size();
    // todo: czy 1ULL jest dobre?
    for (size_t i = 0; i < 1ULL<<m; ++i) {
        auto it = inputs.begin();
        for (int j = (int) (m - 1); j >= 0; j--) {
            signalStates[*it] = i & (1 << j);
            it++;
        }
        eval();
        printSignalsCombination();
    }
    return 0;
}
