// TODO: WAŻNE:
//  poprawić wypisywanie errorów
//  skrócić reada
//  wychodzić z programu z kodem 1 przy błędzie
//  anonimowa przestrzeń nazw dla funkcji pomocniczych
//  pilnować formatowania ręcznie i nie polegać na clionie


// todo: usunąć powyższe komentarze przed wysłaniem


// autorzy: Mateusz Malinowski (mm429561), Paweł Olejnik (po417770)
// data: paździerik 2021
// opis: Plik zawiera implementację rozwiązania zadania 1. Program korzysta
// z wyrażeń regularnych do weryfikowania poprawności wejścia. Układ logiczny
// spełniający specyfikację zadania jest skierowanym grafem acyklicznym.
// Program wykorzystuje algortym sortowania topologicznego w celu optymalnego
// przetwarzania układu.

#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>
#include <stack>
#include <regex>
#include <sstream>

using namespace std;

// Klasa reprezentująca typy bramek logicznych.
enum class GateTypes {
    NOT, OR, NOR, AND, NAND, XOR
};

// Typ reprezentujący sygnał. Sygnał ma numer z zakresu od 1 do 999999999,
// dlatego jest reprezentowany jako nieujemna liczba 32-bitowa.
using signal_t = uint32_t;

// Typ reprezentujący indeks bramki logicznej.
using gate_index_t = size_t;

// Typ reprezentujący bramkę logiczną jako krotkę składającą się z:
// -typu bramki,
// -sygnału wyjściowego,
// -wektora wygnałów wejściowych.
using gate_t = tuple<GateTypes, signal_t, vector<signal_t>>;

// Mapa przechowująca stan sygnałów. Utrzymanie posortowania elementów jest
// istotne ze zwględnu na określoną kolejność wypisywania.
map<signal_t, bool> signalStates;

// Wektor przechowujący bramki logiczne.
vector<gate_t> gates;

// Wektor przechowujący indeksy bramek logicznych w porządku topologicznym.
vector<gate_index_t> topologicalOrder;

// Mapa przechowująca dla każdego sygnału indeksy bramek, dla których ten
// sygnał jest sygnałem wejściowym.
unordered_map<signal_t, vector<gate_index_t>> targetGates;

// Wektor sygnałów wejściowych.
vector<signal_t> inputs;

// Zbiór sygnałów wyjściowych.
unordered_set<signal_t> outputs;

// Funckja zamieniająca nazwę bramki w postaci teksu na odpowiedni typ.
GateTypes parseGateType(const string &s) {
    if (s == "NOT")
        return GateTypes::NOT;
    if (s == "OR")
        return GateTypes::OR;
    if (s == "NOR")
        return GateTypes::NOR;
    if (s == "AND")
        return GateTypes::AND;
    if (s == "NAND")
        return GateTypes::NAND;
    if (s == "XOR")
        return GateTypes::XOR;

    // Z założeń programu ten wyjątek nie zostanie nigdy podniesiony.
    throw exception();
}

// Funckcja sprawdzająca poprawność składniową linii.
// Funkcja zakłada, że liczby zawierające zera wiodące są poprawne.
bool isValidGate(const string &s) {
    regex notGate("\\s*NOT(\\s+0*[1-9]\\d{0,8}){2}\\s*");
    regex xorGate("\\s*XOR(\\s+0*[1-9]\\d{0,8}){3}\\s*");
    regex otherGate("\\s*(AND|NAND|OR|NOR)(\\s+0*[1-9]\\d{0,8}){3,}\\s*");

    return regex_match(s, notGate)
           || regex_match(s, xorGate)
           || regex_match(s, otherGate);
}

void syntaxError(const size_t lineIdx, const string &line) {
    cerr << "Error in line " << lineIdx << ": " << line << endl;
}

void multipleOutputsError(const size_t lineIdx, const signal_t outputSignal) {
    cerr << "Error in line " << lineIdx << ": signal " << outputSignal
         << " is assigned to multiple outputs.\n";
}

// todo: skomentować to i wszystkie pomocnicze które utworzyłeś
void read() {
    size_t lineIdx = 1;   // linie indeksowane od 1 jak w treści
    string line;
    gate_index_t gateIdx = 0;
    bool invalidGates = false;

    while (getline(cin, line)) {
        if (!isValidGate(line)) {   // sprawdzam czy składnia jest poprawna
            syntaxError(lineIdx, line);
            invalidGates = true;
        } else {
            string gateType;
            signal_t outputSignal;
            stringstream ss(line);

            ss >> gateType >> outputSignal;

            if (outputs.count(outputSignal)) {   // sprawdzam czy jest zwarcie
                multipleOutputsError(lineIdx, outputSignal);
                invalidGates = true;
            } else {
                outputs.insert(outputSignal);   // wyjście dodane do setu
            }

            // numery sygnałów wejściowych
            vector<signal_t> inputSignals((istream_iterator<signal_t>(ss)),
                                          istream_iterator<signal_t>());

            // nowa bramka dodana do wektora gates
            gates.emplace_back(parseGateType(gateType), outputSignal,
                               inputSignals);

            signalStates[outputSignal] = false;

            for (const signal_t sig: inputSignals) {
                targetGates[sig].push_back(gateIdx);
                signalStates[sig] = false;
            }
            gateIdx++;
        }
        lineIdx++;
    }

    if (invalidGates) {
        exit(1);
    }
}

// Klasa reprezemtująca status bramki logicznej w trakcie sortowania
// topologicznego.
enum class GateSortingStatus {
    UNVISITED, IN_PROGRESS, DONE
};

// Funkcja pomocnicza sortowania topologicznego. Funkcja zwraca `false` jeśli
// znajdzie cykl (co oznacza, że nie da się posortować topologicznie),
// w przeciwnym wypadku zwraca `true`.
bool topologicalSortHelper(gate_index_t gateIdx,
                           vector<GateSortingStatus> &visited,
                           stack<gate_index_t> &gatesStack) {
    if (visited[gateIdx] == GateSortingStatus::IN_PROGRESS) {
        return false;
    }

    visited[gateIdx] = GateSortingStatus::IN_PROGRESS;

    for (const gate_index_t g: targetGates[get<1>(gates[gateIdx])]) {
        if ((visited[g] == GateSortingStatus::UNVISITED
            && !topologicalSortHelper(g, visited, gatesStack))
            || visited[g] == GateSortingStatus::IN_PROGRESS) {
            return false;
        }
    }

    visited[gateIdx] = GateSortingStatus::DONE;
    gatesStack.push(gateIdx);

    return true;
}

// Sortowanie topologiczne. Funkcja zwraca `true`, jeśli udało się posortować
// topologicznie, w przeciwnym wypadku zwraca `false`.
bool topologicalSort() {
    vector<GateSortingStatus> visited(gates.size(),
                                      GateSortingStatus::UNVISITED);
    stack<gate_index_t> gatesStack;

    for (gate_index_t i = 0; i < gates.size(); i++) {
        if (visited[i] == GateSortingStatus::UNVISITED
            && !topologicalSortHelper(i, visited, gatesStack)) {
            return false;
        }
    }

    while (!gatesStack.empty()) {
        topologicalOrder.push_back(gatesStack.top());
        gatesStack.pop();
    }

    return true;
}

// Funkcja sprawdzająca, które z sygnałów są sygnałami wejściowymi układu.
void findInputs() {
    for (const auto &signal: signalStates) {
        if (!outputs.count(signal.first)) {
            inputs.push_back(signal.first);
        }
    }
}

// Funckja wyliczająca stan wyjściowy bramki OR.
bool evalOr(const vector<signal_t> &signals) {
    for (const signal_t signal: signals) {
        if (signalStates[signal]) {
            return true;
        }
    }
    return false;
}

// Funckja wyliczająca stan wyjściowy bramki AND.
bool evalAnd(const vector<signal_t> &signals) {
    for (const signal_t signal: signals) {
        if (!signalStates[signal]) {
            return false;
        }
    }
    return true;
}

// Funckja wyliczająca stan końcowy układu.
void eval() {
    for (const gate_index_t gateIdx: topologicalOrder) {
        const signal_t outputSignal = get<1>(gates[gateIdx]);
        const vector<signal_t> &inputSignals = get<2>(gates[gateIdx]);

        switch (get<0>(gates[gateIdx])) {
            case GateTypes::NOT:
                signalStates[outputSignal] = !signalStates[inputSignals[0]];
                break;
            case GateTypes::OR:
                signalStates[outputSignal] = evalOr(inputSignals);
                break;
            case GateTypes::NOR:
                signalStates[outputSignal] = !evalOr(inputSignals);
                break;
            case GateTypes::AND:
                signalStates[outputSignal] = evalAnd(inputSignals);
                break;
            case GateTypes::NAND:
                signalStates[outputSignal] = !evalAnd(inputSignals);
                break;
            case GateTypes::XOR:
                signalStates[outputSignal] =
                        signalStates[inputSignals[0]]
                        ^ signalStates[inputSignals[1]];
                break;
        }
    }
}

// Funkcja wypisująca aktualny stan wszystkich sygnałów układu logicznego.
void printCircuitState() {
    for (const auto &signal: signalStates) {
        cout << signal.second;
    }
    cout << endl;
}

int main() {
    read();

    if (!topologicalSort()) {
        cerr << "Error: sequential logic analysis has not yet been "
                "implemented.\n";
        return 1;
    }

    findInputs();

    // Zmienna m oznacza liczbę sygnałów wejściowych tak jak w treści zadania.
    const size_t m = inputs.size();

    // Pętla przetwarza kolejno każdą z 2^m kombinacji stanów sygnałów
    // wejściowych, wylicza dla niej stan układu i wypisuje go na standardowe
    // wyjście.
    for (unsigned long long i = 0; i < 1ULL << m; ++i) {
        auto it = inputs.begin();
        for (int j = (int) (m - 1); j >= 0; j--) {
            signalStates[*it] = i & (1ULL << j);
            it++;
        }
        eval();
        printCircuitState();
    }

    return 0;
}
