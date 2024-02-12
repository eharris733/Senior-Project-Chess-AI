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

private:
    std::vector<TTEntry> table;
};
