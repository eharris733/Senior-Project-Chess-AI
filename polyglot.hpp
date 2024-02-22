#include <fstream>
#include <vector>
#include <random>
#include <string>
#include "chess.hpp"

struct PolyglotEntry {
    uint64_t key;
    uint16_t move;
    uint16_t count;
    uint16_t n;
    uint16_t sum;
};

//code to deal with the book being in bigendian format, and most computers being little endian

// Swap bytes of a 16-bit unsigned integer
uint16_t swapBytes16(uint16_t value) {
    return (value >> 8) | (value << 8);
}

// Swap bytes of a 64-bit unsigned integer
uint64_t swapBytes64(uint64_t value) {
    return ((value >> 56) & 0x00000000000000FF) |
           ((value >> 40) & 0x000000000000FF00) |
           ((value >> 24) & 0x0000000000FF0000) |
           ((value >> 8)  & 0x00000000FF000000) |
           ((value << 8)  & 0x000000FF00000000) |
           ((value << 24) & 0x0000FF0000000000) |
           ((value << 40) & 0x00FF000000000000) |
           ((value << 56) & 0xFF00000000000000);
}

using namespace chess;

class PolyglotBook {
public:
    std::vector<PolyglotEntry> entries;

    PolyglotBook(const std::string& filename) {
        loadBook(filename);
    }

    //modified to swap endianness
    void loadBook(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open Polyglot book: " << filename << std::endl;
        return;
    }

    PolyglotEntry entry;
    while (file.read(reinterpret_cast<char*>(&entry), sizeof(PolyglotEntry))) {
        entry.key = swapBytes64(entry.key);
        entry.move = swapBytes16(entry.move);
        entry.count = swapBytes16(entry.count);
        entry.n = swapBytes16(entry.n);
        entry.sum = swapBytes16(entry.sum);
        entries.push_back(entry);
    }
    printf("Loaded %lu entries from %s\n", entries.size(), filename.c_str());
}


    Move polyglotMoveToEngineMove(uint16_t move, const Board& board) {
        // Decompose the polyglot move to from and to squares
        int from = (move >> 6) & 0x3F;
        int to = move & 0x3F;

        // Here you should convert 'from' and 'to' to your engine's Move format
        // This might involve creating a new Move object with these details.
        // For example, if your Move constructor does not directly take 'from' and 'to',
        // you might need to find a corresponding function or method in your engine that does this conversion.
        // This is a placeholder for the actual conversion logic.
        Move engineMove = Move(move);

        printf("Polyglot move: %s\n", uci::moveToUci(engineMove).c_str());

        return engineMove;
        // // Verify the move is legal in the current position
        // if (board.isLegalMove(engineMove)) {
        //     return engineMove;
        // }

        //return Move::NULL_MOVE; // Return null move if conversion fails or move is illegal
    }

    Move pickRandomMove(const Board& board) {
        uint64_t key = board.zobrist();
        std::vector<Move> possibleMoves;
        for (const auto& entry : entries) {
            if (entry.key == key) {
                Move move = polyglotMoveToEngineMove(entry.move, board);
                if (move != Move::NULL_MOVE) {
                    possibleMoves.push_back(move);
                }
            }
        }

        if (possibleMoves.empty()) {
            return Move::NULL_MOVE;
        } else {
            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_int_distribution<size_t> dist(0, possibleMoves.size() - 1);
            return possibleMoves[dist(mt)];
        }
    }
};
