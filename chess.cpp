//This file is where anything having to do with raw feature extraction is done. 
// We have a FeatureExtractor Class, which takes in a Board object, and then extracts all the features from the current position
// Built in is also a Feature tester class, which contians a list of FEN strings, and their associated expected features
// we extract features from each FEN, and see if they match up with the expected test results. If not, we can delve further to try 
// and make the feature extraction bullet-proof. Optimization will probably be a secondary concern
// optimization possibilities:
    // detecting for lazy evaluation
    // not recalculating attacks, and certain bitboards

#include "chess.hpp"


using namespace chess;
// command to run in terminal: g++ -std=c++17 -O3 -march=native -o example chess.cpp

// a list of features to extract, based from the 2008 paper,
// also contains the values they determined, but mine probably won't be the same
// right now, I will focus on optimal extractions using bit operations

// Structure to hold expected feature values for a FEN
struct Features {
    float pawns; // The value of Pawn's on their respective square
    float knights; // The value of Kinght's on their respective square
    float bishops; // The value of Bishop's on their respective square
    float rooks; // The value of Rook's on their respective square
    float queens; // The value of Queen's on their respective square
    float passedPawnMult; // Pawn with no other pawns in its way
    float doubledPawnPenalty; // Pawns stacked on the same file
    float isolatedPawnPenalty; // Pawn with no adjacent pawns
    float backwardPawnPenalty; // Pawn with all adjacent pawns advanced
    float weakSquarePenalty; // Square on your side where no pawns can attack
    float passedPawnEnemyKingSquare; // Rule of the square
    float knightOutpostMult; // Knight is on a weak square
    float bishopMobility; // How many squares a bishop can move to
    float bishopPair; // Possessing both colored bishops
    float rookAttackKingFile; // Rook is on same file as enemy king
    float rookAttackKingAdjFile; // Rook is on adjacent file to enemy king
    float rook7thRank; // Rook is on second to last rank
    float rookConnected; // Rooks can guard each other
    float rookMobility; // How many squares a rook has
    float rookBehindPassedPawn; // Rook is behind a passed pawn
    float rookOpenFile; // Rook has no pawns in its file
    float rookSemiOpenFile; // Rook has no friendly pawns in its file
    float rookAtckWeakPawnOpenColumn; // Rook can directly attack a weak enemy pawn
    float kingFriendlyPawn; // How many and how close are friendly pawns
    float kingNoEnemyPawnNear; // King is far away from enemy pawns
    float kingPressureMult; // Multiplier for king pressure
};


class FeatureExtractor {
private:
    std::unordered_map<std::string, std::function<Bitboard(const Board&)>> features;

public:
    FeatureExtractor() {
        // Register all features and their extraction functions here
        features["pawns"] = [](const Board& board) -> Bitboard {
            return countPiece(board, PieceType::PAWN, Color::WHITE) - countPiece(board, PieceType::PAWN, Color::BLACK);
        };
        features["knights"] = [](const Board& board) -> Bitboard {
            return countPiece(board, PieceType::KNIGHT, Color::WHITE) - countPiece(board, PieceType::KNIGHT, Color::BLACK);
        };
        features["bishops"] = [](const Board& board) -> Bitboard {
            return countPiece(board, PieceType::BISHOP, Color::WHITE) - countPiece(board, PieceType::BISHOP, Color::BLACK);
        };
        features["rooks"] = [](const Board& board) -> Bitboard {
            return countPiece(board, PieceType::ROOK, Color::WHITE) - countPiece(board, PieceType::ROOK, Color::BLACK);
        };
        features["queens"] = [](const Board& board) -> Bitboard {
            return countPiece(board, PieceType::QUEEN, Color::WHITE) - countPiece(board, PieceType::QUEEN, Color::BLACK);
        };
        features["passedPawnMult"] = [](const Board& board) -> Bitboard {
            return detectPassedPawns(board, Color::WHITE) - detectPassedPawns(board, Color::BLACK);
        };
        
        features["doubledPawnPenalty"] = [](const Board& board) -> Bitboard {
            return detectDoubledPawns(board, Color::WHITE) - detectDoubledPawns(board, Color::BLACK);
        };
        features["isolatedPawnPenalty"] = [](const Board& board) -> Bitboard {
            return detectIsolatedPawns(board, Color::WHITE) - detectIsolatedPawns(board, Color::BLACK);
        };
        features["backwardPawnPenalty"] = [](const Board& board) -> Bitboard {
            return detectBackwardPawns(board, Color::WHITE) - detectBackwardPawns(board, Color::BLACK);
        };
    }

    void extractAndCompare(const std::vector<std::string>& fens, const std::vector<Features>& expectedFeatures) {
        for (size_t i = 0; i < fens.size(); ++i) {
            Board board(fens[i]);
            for (const auto& feature : features) {
                std::cout << "Extracting feature " << feature.first << " for FEN " << fens[i] << std::endl;
                float result = feature.second(board);
                std::cout << "Result: " << result << std::endl;
                std::cout << "Expected: " << expectedFeatures[i] << std::endl;
                std::cout << "Result == Expected: " << (result == expectedFeatures[i]) << std::endl;
            }
        }
    }
};




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






int detectPassedPawns(const Board& board, Color col) {
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







int detectDoubledPawns(const Board& board, Color color) {
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


int detectIsolatedPawns(const Board& board, Color color) {
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
int countPiece(const Board& board, PieceType pieceType, Color color){
    Bitboard b = board.pieces(pieceType, color);
    int count = 0;

    // Count the number of bits set in the bitboard
    while (b) {
        count += b & 1; // Increment count if the least significant bit is 1
        b >>= 1;        // Shift bits to the right
    }

    std::cout << count << pieceTypeToChar(pieceType) << "s for side " << (color == Color::WHITE ? "White" : "Black") << std::endl;
    return count;
}

int detectBackwardPawns(const Board& board, Color color) {
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





// prints out every move from a given position
int main () {

    //4k3/7P/P7/p7/8/8/4K2p/8 w - - 0 1 should have one passed pawn for each
    //rnbqkb1r/1p2p1p1/1p2p2n/1p1P2p1/3P4/3P2P1/P2P2P1/RNBQKBNR w KQkq - 0 1
    Board board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    detectPassedPawns(board, Color::WHITE);
    detectPassedPawns(board, Color::BLACK);

    detectDoubledPawns(board, Color::WHITE);
    detectDoubledPawns(board, Color::BLACK);

    detectIsolatedPawns(board, Color::WHITE);
    detectIsolatedPawns(board, Color::BLACK);

    detectBackwardPawns(board, Color::WHITE);
    detectBackwardPawns(board, Color::BLACK);

    countPiece(board, PieceType::PAWN, Color::WHITE);
    countPiece(board, PieceType::BISHOP, Color::WHITE);
    countPiece(board, PieceType::KNIGHT, Color::WHITE);
    countPiece(board, PieceType::ROOK, Color::WHITE);
    countPiece(board, PieceType::KING, Color::WHITE);
    countPiece(board, PieceType::QUEEN, Color::WHITE);

    countPiece(board, PieceType::PAWN, Color::BLACK);
    countPiece(board, PieceType::BISHOP, Color::BLACK);
    countPiece(board, PieceType::KNIGHT, Color::BLACK);
    countPiece(board, PieceType::ROOK, Color::BLACK);
    countPiece(board, PieceType::KING, Color::BLACK);
    countPiece(board, PieceType::QUEEN, Color::BLACK);

    return 0;
    
}