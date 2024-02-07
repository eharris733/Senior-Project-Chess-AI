#pragma once
#include <vector>
#include <optional>
#include "chess.hpp"


using namespace chess;
using namespace std;


// the kinds of nodes that can be stored in the table
enum class NodeType {
    EXACT,
    LOWERBOUND, // alpha node
    UPPERBOUND  // beta node
};
//struct to help the organization of the table
struct TTEntry {
    uint64_t zobristKey;
    int depth;
    int score;
    NodeType nodeType;
    Move bestMove;
};

class TranspositionTable {
public:
    TranspositionTable(size_t size) {
        table.resize(size);
    }

    void save(uint64_t zobristKey, int depth, int score, NodeType nodeType, Move bestMove) {
        size_t index = zobristKey % table.size();
        table[index] = {zobristKey, depth, score, nodeType, bestMove};
    }

    std::optional<TTEntry> retrieve(uint64_t zobristKey) {
        size_t index = zobristKey % table.size();
        if (table[index].zobristKey == zobristKey) {
            return table[index];
        }
        return {};
    }

    //necceary to clear the table
    void clear() {
    for (auto& entry : table) {
        entry = TTEntry{}; // Reset each entry. Assumes TTEntry is default-constructible.
    }
}


private:
    std::vector<TTEntry> table;
};
