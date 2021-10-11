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
// TODO: WAŻNE:
//  poprawić komentarze
//  poprawić wypisywanie errorów
//  skrócić reada
//  wychodzić z programu z kodem 1 przy błędzie
//  anonimowa przestrzeń nazw dla funkcji pomocniczych
//  pilnować formatowania ręcznie i nie polegać na clionie

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

// klasa reprezentująca typy bramek logicznych
enum class GateTypes {
    NOT, OR, NOR, AND, NAND, XOR
};

// sygnały mają nr od 1 do 999999999 więc potrzeba conajmniej 32 bitowego typu
using signal_t = uint32_t;
using gate_index_t = size_t;
// <TYP, wyjście, wejścia>
using gate_t = tuple<GateTypes, signal_t, vector<signal_t>>;

// przechowuje stan sygnałów w danej chwili
// potrzebna jest mapa, bo na kóncu wypisujemy posortowane
map<signal_t, bool> signalStates;

// wektor przechowujący wszystkie bramki
// bramki indeksujemy od 0
vector<gate_t> gates;

// bramki posortowane topologicznie
vector<gate_index_t> topologicalOrder;

// dla każdego sygnału musimy pamiętać do jakich bramek wchodzi
// na wektorze trzymamy indeks bramki
unordered_map<signal_t, vector<gate_index_t>> targetGates;

// zbiór sygnałów wyjściowych
set<signal_t> inputs;

// zbiór sygnałów wyjściowych
unordered_set<signal_t> outputs;

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

    throw exception();
}

// sprawdza poprawność składniową linii
// funkcja zakłada że liczba postaci 00123 jest dobra
bool isValidGate(const string &s) {
    regex notGate("\\s*NOT(\\s+0*[1-9]\\d{0,8}){2}\\s*");
    regex xorGate("\\s*XOR(\\s+0*[1-9]\\d{0,8}){3}\\s*");
    regex otherGate("\\s*(AND|NAND|OR|NOR)(\\s+0*[1-9]\\d{0,8}){3,}\\s*");

    return regex_match(s, notGate)
           || regex_match(s, xorGate)
           || regex_match(s, otherGate);
}

void read() {
    size_t lineIdx = 1;   // linie indeksowane od 1 jak w treści
    string line;
    gate_index_t gateIdx = 0;
    bool invalidGates = false;

    while (getline(cin, line)) {
        if (!isValidGate(line)) {   // sprawdzam czy składnia jest poprawna
            cerr << "Error in line " << lineIdx << ": " << line << endl;
            invalidGates = true;
        } else {
            string gateType;
            signal_t outputSignal;
            stringstream ss(line);

            ss >> gateType >> outputSignal;

            if (outputs.count(outputSignal)) {   // sprawdzam czy jest zwarcie
                cerr << "Error in line " << lineIdx << ": signal " <<
                     outputSignal << " is assigned to multiple outputs.\n";
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

enum class GateSortingStatus {
    UNVISITED, IN_PROGRESS, DONE
};

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

bool topologicalSort() {
    vector<GateSortingStatus> visited(gates.size());
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

void findInputs() {
    for (const auto &signal: signalStates) {
        if (!outputs.count(signal.first)) {
            inputs.insert(signal.first);
        }
    }
}

bool evalOr(const vector<signal_t> &signals) {
    for (const signal_t signal: signals) {
        if (signalStates[signal]) {
            return true;
        }
    }
    return false;
}

bool evalAnd(const vector<signal_t> &signals) {
    for (const signal_t signal: signals) {
        if (!signalStates[signal]) {
            return false;
        }
    }
    return true;
}

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

// wypisywanie wyjśćia
void printSignalsCombination() {
    for (const auto &el: signalStates) {
        cout << el.second;
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

    const size_t m = inputs.size();

    for (unsigned long long i = 0; i < 1ULL << m; ++i) {
        auto it = inputs.begin();
        for (int j = (int) (m - 1); j >= 0; j--) {
            signalStates[*it] = i & (1ULL << j);
            it++;
        }
        eval();
        printSignalsCombination();
    }

    return 0;
}
