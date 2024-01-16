//This file is where anything having to do with raw feature extraction is done. 
// We have a FeatureExtractor Class, which takes in a Board object, and then extracts all the features from the current position
// Built in is also a Feature tester class, which contians a list of FEN strings, and their associated expected features
// we extract features from each FEN, and see if they match up with the expected test results. If not, we can delve further to try 
// and make the feature extraction bullet-proof. Optimization will probably be a secondary concern
// optimization possibilities:
    // detecting for lazy evaluation
    // not recalculating attacks, and certain bitboards

// important distinction: These are the raw features, not the features weighted by the evaluation function, that is a whole different story. 

#include "chess.hpp"
#include "features.hpp" 

using namespace chess;
// command to run in terminal: g++ -std=c++17 -O3 -march=native -o example chess.cpp


class FeatureExtractor {
private:
    Features features;
    const Board& board;

    //helper functions
    Color opposite_color(Color color) {
    return (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
}

Bitboard square_to_bitmap(Square sq) {
    return Bitboard(1) << sq; // Shifts the 1 to the position of the square
}

constexpr int rank_of(Square sq) {
    return static_cast<int>(sq) / 8; // Divide the square index by 8 to get the rank
}

constexpr int file_of(Square sq) {
    return static_cast<int>(sq) % 8; // Modulo the square index by 8 to get the file
}

// Helper function to convert file index to string representation (a-h)
std::string fileToString(int file) {
    // Assuming file is 0-7 for a-h
    return std::string(1, 'a' + file);
}

// Helper function to create a bitboard for a specific file
Bitboard fileBitboard(int file) {
    Bitboard mask = 0x0101010101010101; // Bitboard with the a-file set
    return mask << file; // Shift to the appropriate file
}

Bitboard forward_squares(Square sq, Color col) {
    Bitboard mask = 0;
    int rank = rank_of(sq), file = file_of(sq);
    while ((col == Color::WHITE && rank < 7) || (col == Color::BLACK && rank > 0)) {
        rank += (col == Color::WHITE) ? 1 : -1;
        for (int f = std::max(0, file - 1); f <= std::min(7, file + 1); ++f) {
            mask |= square_to_bitmap(Square(rank * 8 + f));
        }
    }
    return mask;
}

Bitboard behind_squares(Square sq, Color col) {
    Bitboard mask = 0;
    int rank = rank_of(sq), file = file_of(sq);
    while ((col == Color::WHITE && rank > 0) || (col == Color::BLACK && rank < 7)) {
        rank += (col == Color::WHITE) ? -1 : 1;
        for (int f = std::max(0, file - 1); f <= std::min(7, file + 1); ++f) {
            mask |= square_to_bitmap(Square(rank * 8 + f));
        }
    }
    return mask;
}

Bitboard blocking_squares(Square sq, Color col, Bitboard enemyPawns) {
    Bitboard mask = 0;
    int rank = rank_of(sq), file = file_of(sq);

    // Calculate squares in front of the pawn including the file and adjacent files
    int startRank = col == Color::WHITE ? rank + 1 : rank - 1;
    int endRank = col == Color::WHITE ? 7 : 0;
    int rankStep = col == Color::WHITE ? 1 : -1;

    for (int r = startRank; (col == Color::WHITE ? r <= endRank : r >= endRank); r += rankStep) {
        for (int f = std::max(0, file - 1); f <= std::min(7, file + 1); ++f) {
            if (file != f || r != rank) { // Skip the pawn's own square
                mask |= square_to_bitmap(Square(r * 8 + f));
            }
        }
    }

    return mask;
}

Bitboard wAttackFrontSpans(Bitboard wpawns) {
    Bitboard leftAttacks = attacks::pawnLeftAttacks<Color::WHITE>(wpawns);
    Bitboard  rightAttacks = attacks::pawnRightAttacks<Color::WHITE>(wpawns);
    Bitboard attackSpan = 0;

    while (leftAttacks | rightAttacks) {
        attackSpan |= leftAttacks | rightAttacks;
        leftAttacks <<= 7;
        rightAttacks <<= 9;
    }

    return attackSpan;
}

Bitboard bAttackFrontSpans(Bitboard bpawns) {
    Bitboard leftAttacks = attacks::pawnLeftAttacks<Color::BLACK>(bpawns);
    Bitboard  rightAttacks = attacks::pawnRightAttacks<Color::BLACK>(bpawns);
    Bitboard attackSpan = 0;

    while (leftAttacks | rightAttacks) {
        attackSpan |= leftAttacks | rightAttacks;
        leftAttacks >>= 9;
        rightAttacks >>= 7;
    }

    return attackSpan;
}

Bitboard wPawnAttacks(Bitboard wpawns) {
    Bitboard leftAttacks = attacks::pawnLeftAttacks<Color::WHITE>(wpawns);
    Bitboard  rightAttacks = attacks::pawnRightAttacks<Color::WHITE>(wpawns);
    return leftAttacks | rightAttacks;
}


Bitboard bPawnAttacks(Bitboard bpawns) {
    Bitboard leftAttacks = attacks::pawnLeftAttacks<Color::BLACK>(bpawns);
    Bitboard  rightAttacks = attacks::pawnRightAttacks<Color::BLACK>(bpawns);
    return leftAttacks | rightAttacks;
}

// feature extraction functions

int detectPassedPawns(Color col) {
    Bitboard pawns = board.pieces(PieceType::PAWN, col);
    Bitboard enemyPawns = board.pieces(PieceType::PAWN, opposite_color(col));

    int passedPawns = 0;
    // Check each pawn for passed pawn status
    while (pawns) {
        Square pawnSquare = chess::builtin::poplsb(pawns); // Get the least significant bit as Square
        Bitboard blockingSquares = blocking_squares(pawnSquare, col, enemyPawns);

        // Check if no enemy pawns are on the blocking squares
        if ((blockingSquares & enemyPawns) == 0) {
            std::cout << "Passed pawn at square " << squareToString[pawnSquare] << " for " << (col == Color::WHITE ? "White" : "Black") << std::endl;
            if (col == Color::WHITE) {
                passedPawns ++;
            }
            else {
                passedPawns --;
            }
        }
    }
    return passedPawns;
}







int detectDoubledPawns(Color color) {
    Bitboard pawns = board.pieces(PieceType::PAWN, color);
    Bitboard checkedFiles = 0; // Bitboard to keep track of files that have been checked
    
    int doubledPawns = 0;

    for (int file = 0; file < 8; ++file) {
        Bitboard fileMask = fileBitboard(file);
        if ((checkedFiles & fileMask) == 0) { // Check if the file hasn't been checked yet
            Bitboard pawnsOnFile = pawns & fileMask;

            if (chess::builtin::popcount(pawnsOnFile) > 1) {
                // There are doubled pawns on this file
                std::cout << "Doubled pawns on file " << fileToString(file) << " for " << (color == Color::WHITE ? "White" : "Black") << std::endl;
                if (color == Color::WHITE) {
                    doubledPawns ++;
                }
                else {
                    doubledPawns --;
                }
            }

            checkedFiles |= fileMask; // Mark this file as checked
        }
    }
    return doubledPawns;
}


int detectIsolatedPawns(Color color) {
    Bitboard pawns = board.pieces(PieceType::PAWN, color);

    int isolatedPawns = 0;

    // Iterate over each pawn
    while (pawns) {
        Square pawnSquare = chess::builtin::poplsb(pawns); // Get the least significant bit as Square
        int file = file_of(pawnSquare); // Get the file of the pawn (0 to 7 for a-h)

        // Get bitboards for the adjacent files
        Bitboard leftFileMask = file > 0 ? fileBitboard(file - 1) : 0;
        Bitboard rightFileMask = file < 7 ? fileBitboard(file + 1) : 0;

        // Bitboard of all friendly pawns on the adjacent files
        Bitboard adjacentPawns = board.pieces(PieceType::PAWN, color) & (leftFileMask | rightFileMask);

        // Check if there are no friendly pawns on adjacent files
        if (chess::builtin::popcount(adjacentPawns) == 0) {
            std::cout << "Isolated pawn at square " << squareToString[pawnSquare] << " for " << (color == Color::WHITE ? "White" : "Black") << std::endl;
            if (color == Color::WHITE) {
                isolatedPawns ++;
            }
            else {
                isolatedPawns --;
            }
        }
    }
    return isolatedPawns;
}

// Other helper functions (fileBitboard, file_of, squareToString, etc.) remain the same


// change to incorporate piece tables soon
std::vector<Square> findPieceSquares(PieceType pieceType, Color color) {
    std::vector<Square> pieceSquares;

    Bitboard pieces = board.pieces(pieceType, color);

    // Iterate over each piece
    while (pieces) {
        Square pieceSquare = chess::builtin::poplsb(pieces); // Get the least significant bit as Square
        pieceSquares.push_back(pieceSquare);
    }

    return pieceSquares;
}


int detectBackwardPawns(Color color) {
    int count = 0;
    Bitboard pawns = board.pieces(PieceType::PAWN, color);
    Bitboard enemyPawns = board.pieces(PieceType::PAWN, opposite_color(color));

    Bitboard stops = (color == Color::WHITE) ? pawns << 8 : pawns >> 8;
    Bitboard attackSpans = (color == Color::WHITE) ? wAttackFrontSpans(pawns) : bAttackFrontSpans(pawns);
    Bitboard enemyAttacks = (color == Color::WHITE) ? bPawnAttacks(enemyPawns) : wPawnAttacks(enemyPawns);

    Bitboard backwardPawns = (stops & enemyAttacks & ~attackSpans);
    backwardPawns = (color == Color::WHITE) ? backwardPawns >> 8 : backwardPawns << 8;

    while (backwardPawns) {
        Square pawnSquare = chess::builtin::poplsb(backwardPawns);
        std::cout << "Backward pawn at square " << squareToString[pawnSquare] << " for " << (color == Color::WHITE ? "White" : "Black") << std::endl;
        if (color == Color::WHITE) {
            count ++;
        }
        else {
            count --;
        }
    }
    return count;
}

public:
    FeatureExtractor(const Board& board) : board(board) {
    }

    Features getFeatures() {
        return features;
    }

    void extract() {
        // Extract the features
        features.wpawns = findPieceSquares(PieceType::PAWN, Color::WHITE);
        features.bpawns = findPieceSquares(PieceType::PAWN, Color::BLACK);
        features.wknights = findPieceSquares(PieceType::KNIGHT, Color::WHITE);
        features.bknights = findPieceSquares(PieceType::KNIGHT, Color::BLACK);
        features.wbishops = findPieceSquares(PieceType::BISHOP, Color::WHITE);
        features.bbishops = findPieceSquares(PieceType::BISHOP, Color::BLACK);
        features.wrooks = findPieceSquares(PieceType::ROOK, Color::WHITE);
        features.brooks = findPieceSquares(PieceType::ROOK, Color::BLACK);
        features.wqueen = findPieceSquares(PieceType::QUEEN, Color::WHITE);
        features.bqueen = findPieceSquares(PieceType::QUEEN, Color::BLACK);
        features.wking = findPieceSquares(PieceType::KING, Color::WHITE);
        features.bking = findPieceSquares(PieceType::KING, Color::BLACK);
        
        features.backwardPawns = detectBackwardPawns(Color::WHITE) - detectBackwardPawns(Color::BLACK);
        features.doubledPawns = detectDoubledPawns(Color::WHITE) - detectDoubledPawns(Color::BLACK);
        features.isolatedPawns = detectIsolatedPawns(Color::WHITE) - detectIsolatedPawns(Color::BLACK);
        features.passedPawns = detectPassedPawns(Color::WHITE) - detectPassedPawns(Color::BLACK);
    }


}; // end of FeatureExtractor class





