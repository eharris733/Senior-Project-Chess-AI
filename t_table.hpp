// note, this is a bit over my head in terms of how it all works
// mostly stolen from chat gpt's suggestions

//note also thats its a vector as the data structure, perhaps a fixed sized array would be better?
#include <vector>
#include <optional>
#include "chess.hpp"

using namespace chess;
using namespace std;

enum class NodeType {
    EXACT,
    LOWERBOUND,
    UPPERBOUND
};

struct TTEntry {
    uint64_t zobristKey = 0;
    int depth = -1; // Use -1 to indicate an uninitialized entry
    int score = 0;
    NodeType nodeType;
    Move bestMove;
    bool valid = false; // Indicates if the entry is valid
};

class TranspositionTable {
public:
    TranspositionTable(size_t size) : table(size) {}

    void save(uint64_t zobristKey, int depth, int score, NodeType nodeType, Move bestMove) {
        size_t index = zobristKey % table.size();
        // Depth-preferred replacement strategy
        if (!table[index].valid || table[index].depth < depth) {
            table[index] = {zobristKey, depth, score, nodeType, bestMove, true};
        }
    }

    std::optional<TTEntry> retrieve(uint64_t zobristKey) {
        size_t index = zobristKey % table.size();
        if (table[index].valid && table[index].zobristKey == zobristKey) {
            return table[index];
        }
        return {};
    }

    void clear() {
        for (auto& entry : table) {
            entry.valid = false; // Mark all entries as invalid
        }
    }

    void debugSize() {
        cout << "Table size: " << table.size() << endl;
        cout << "Percent full: " << (count_if(table.begin(), table.end(), [](const TTEntry& entry) { return entry.valid; }) / (double)table.size()) * 100 << "%\n " << endl;
    }

private:
    std::vector<TTEntry> table;
};
