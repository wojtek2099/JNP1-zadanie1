#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

enum class GateTypes {
    NOT, OR, NOR, AND, NAND, XOR
};

using Gate = tuple<GateTypes, int, vector<int>>;
// przechowuje stan inputu w danej chwili
unordered_map<int, bool> inputs;
vector<Gate> gates;
vector<int> visited;

// to się zaklepie na końcu
void wczytaj() {

}

bool checkSequentialHelper(size_t gateIndex) {
    if (visited[gateIndex]) {
        return true;
    }

}

bool checkSequential() {
    for (auto x : gates) {

    }
}

int main() {
    wczytaj();
    if (checkSequential()) {
        cerr << "Error: sequential logic analysis has not yet been implemented.";
        return 0;
    }

    return 0;
}
