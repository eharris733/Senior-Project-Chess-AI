//This file is where anything having to do with raw feature extraction is done. 
// We have a FeatureExtractor Class, which takes in a Board object, and then extracts all the features from the current position
// Built in is also a Feature tester class, which contians a list of FEN strings, and their associated expected features
// we extract features from each FEN, and see if they match up with the expected test results. If not, we can delve further to try 
// and make the feature extraction bullet-proof. Optimization will probably be a secondary concern
// ok future elliot here, we optomized by caching results, and it was a LOT faster, so we need more optomizations to make this computer as effecient as possible

// TO DO: 
    // 1. Add piece tables to the feature extraction (since we calculate, then we iterate, why not calculate while iterating?)
    // 2. no more vectors!! only bitboards, and then we can use the popcount function to get the number of pieces
    // 3. conditional recalculation of features, if a move is made, we only need to recalculate the features that are affected by the move

// Once this is done, we can expose our evaluation weights to a sepearate file/config, as well as our magically 
// tuned search parameters, and then the GA can try and tune those parameters.

// new ideal file structure: extract and weight at the same time using config file

// important distinction: These are the raw features, not the features weighted by the evaluation function, that is a whole different story. 

#include "chess.hpp"
#include "features.hpp" 
#pragma once

using namespace chess;
using namespace std;


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

Bitboard rankBitboard(int rank) {
    // Bitboard with the 1s on the 1st rank
    Bitboard mask = 0xFF;
    return mask << (rank * 8); // Shift to the appropriate rank
}

Square int_to_square(int sq) {
    return static_cast<Square>(sq);
}

Square bitboard_to_square(Bitboard bitboard) {
    return int_to_square(__builtin_ctzll(bitboard));
}

Bitboard inBetween(Square sq1, Square sq2) {
    Bitboard between = 0ULL;
    int rank1 = rank_of(sq1), file1 = file_of(sq1);
    int rank2 = rank_of(sq2), file2 = file_of(sq2);

    if (rank1 == rank2) {
        // Same Rank
        for (int file = std::min(file1, file2) + 1; file < std::max(file1, file2); ++file) {
            between |= 1ULL << (rank1 * 8 + file);
        }
    } else if (file1 == file2) {
        // Same File
        for (int rank = std::min(rank1, rank2) + 1; rank < std::max(rank1, rank2); ++rank) {
            between |= 1ULL << (rank * 8 + file1);
        }
    }
    // If not on the same rank or file, returns 0 (no in-between squares)
    return between;
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





Bitboard detectPassedPawns(Color col, Bitboard wPawnSquares, Bitboard bPawnSquares) {
    Bitboard pawnSquares = col == Color::WHITE ? wPawnSquares : bPawnSquares;
    Bitboard enemyPawns = col == Color::WHITE ? bPawnSquares : wPawnSquares;
    Bitboard passedPawns = 0;
    int forwardShift = col == Color::WHITE ? 8 : -8;

    while (pawnSquares) {
        Bitboard pawnSquare = pawnSquares & -pawnSquares;

        Bitboard blockingSquares = 0;
        Bitboard currentSquare = pawnSquare;
        do {
            currentSquare = (col == Color::WHITE) ? (currentSquare << 8) : (currentSquare >> 8);
            blockingSquares |= currentSquare;
        } while (currentSquare && (currentSquare & enemyPawns) == 0 && !(currentSquare & (col == Color::WHITE ? 0xFF00000000000000LL : 0xFFLL)));

        if ((blockingSquares & enemyPawns) == 0 && currentSquare) {
            passedPawns |= pawnSquare;
        }

        pawnSquares &= pawnSquares - 1;
    }

    return passedPawns;
}


int detectDoubledPawns(Color color, Bitboard wpawns, Bitboard bpawns) {
    const auto& pawns = (color == Color::WHITE) ? wpawns : bpawns;
    int doubledPawns = 0;

    for (int file = 0; file < 8; ++file) {
        Bitboard fileMask = fileBitboard(file);
        Bitboard pawnsOnFile = pawns & fileMask;

        int count = chess::builtin::popcount(pawnsOnFile);
        if (count > 1) {
            doubledPawns += (color == Color::WHITE) ? count - 1 : -(count - 1);
        }
    }
    return doubledPawns;
}


Bitboard detectIsolatedPawns(Bitboard pawns) {
    Bitboard isolatedPawns = 0;

    // Iterate through each square on the board to check for isolated pawns
    for (int sq = 0; sq < 64; ++sq) {
        Bitboard squareBitboard = Bitboard(1) << sq; // Create a bitboard for the current square
        if (!(squareBitboard & pawns)) continue; // Skip if there's no pawn of the given color on this square

        int file = sq % 8; // Calculate file index (0-7) from square index (0-63)
        Bitboard leftFileMask = file > 0 ? fileBitboard(file - 1) : 0;
        Bitboard rightFileMask = file < 7 ? fileBitboard(file + 1) : 0;
        Bitboard adjacentPawns = pawns & (leftFileMask | rightFileMask);

        // If there are no pawns on adjacent files, mark this pawn as isolated
        if (adjacentPawns == 0) {
            isolatedPawns |= squareBitboard;
        }
    }
    return isolatedPawns;
}

Bitboard detectWeakPawns(Color color, Bitboard wpawns, Bitboard bpawns, Bitboard wAttacks, Bitboard bAttacks) {
    const Bitboard pawns = (color == Color::WHITE) ? wpawns : bpawns;
    const Bitboard enemyPawns = (color == Color::WHITE) ? bpawns : wpawns;
    const Bitboard friendlyAttacks = (color == Color::WHITE) ? wAttacks : bAttacks;
    const Bitboard enemyAttacks = (color == Color::WHITE) ? bAttacks : wAttacks;
    Bitboard backwardsPawns = 0;

    Bitboard stops, wAttackSpans, bAttackSpans, backwardPawns;

    if (color == Color::WHITE) {
        stops = wpawns << 8;
        backwardPawns = (stops & enemyAttacks & ~friendlyAttacks) >> 8;
    } else {
        stops = bpawns >> 8;
        backwardPawns = (stops & enemyAttacks & ~friendlyAttacks) << 8;
    }

    return backwardsPawns;
}




Bitboard detectWeakSquares(Color color, Bitboard wpawns, Bitboard bpawns, Bitboard wAttacks, Bitboard bAttacks) {
    // Directly utilize the provided cached bitboards
    Bitboard pawns = (color == Color::WHITE) ? wpawns : bpawns;
    Bitboard attacks = (color == Color::WHITE) ? wAttacks : bAttacks;
    Bitboard weakSquares;
    Bitboard potentialPawnCoverage = 0; // Declare the variable 'potentialPawnCoverage' and initialize it to 0.

    for (Bitboard p = wpawns; p; p &= p - 1) {
        Bitboard pos = p & -p; // isolate the least significant bit (current pawn position)
        potentialPawnCoverage |= (pos << 8) | ((pos & ~fileBitboard(0)) << 7) | ((pos & ~fileBitboard(7)) << 9);
    }

    // For black pawns: cover squares directly ahead, and one square diagonally ahead to the left and right
    for (Bitboard p = bpawns; p; p &= p - 1) {
        Bitboard pos = p & -p; // isolate the least significant bit (current pawn position)
        potentialPawnCoverage |= (pos >> 8) | ((pos & ~fileBitboard(0)) >> 9) | ((pos & ~fileBitboard(7)) >> 7);
    }

    // Define target ranks for weak square detection
    Bitboard targetRanks;
    if (color == Color::WHITE) {
        targetRanks = rankBitboard(3) | rankBitboard(4) | rankBitboard(5);
        // Weak squares are those on target ranks not covered by any potential pawn move or attack
        weakSquares = targetRanks & ~potentialPawnCoverage & ~wpawns;
    } else {
        targetRanks = rankBitboard(2) | rankBitboard(3) | rankBitboard(4);
        weakSquares = targetRanks & ~potentialPawnCoverage & ~bpawns;
    }

    return weakSquares;
}




int ruleOfTheSquare(Color color, Bitboard passedPawns, Bitboard king) {
    Square kSquare = bitboard_to_square(king);

    int count = 0;
    while (passedPawns) {
        Bitboard passedPawnSquareBit = passedPawns & -passedPawns; // Extract LSB (a passed pawn)
        Square passedPawnSquare = bitboard_to_square(passedPawnSquareBit); // Convert the bit to its square

        int passedPawnRank = rank_of(passedPawnSquare);
        int passedPawnFile = file_of(passedPawnSquare);

        // Calculate the "square" of the pawn based on its rank and the direction it advances
        int promotionRank = (color == Color::WHITE) ? 7 : 0; // Rank the pawn needs to reach to promote
        int distanceToPromotion = abs(promotionRank - passedPawnRank);

        int kingRank = rank_of(kSquare);
        int kingFile = file_of(kSquare);

        // Calculate the king's distance to the promotion path of the pawn
        int distanceToStopPromotion = max(abs(kingRank - passedPawnRank), abs(kingFile - passedPawnFile));

        // If the king can reach the pawn's promotion path before or as it promotes, count it
        if (distanceToStopPromotion <= distanceToPromotion) {
            count++;
        }

        passedPawns &= passedPawns - 1; // Remove this pawn from the set
    }
    return count;
}


// weak squares are the opposite of the color
int knightOutposts(Color color, Bitboard weakSquares, Bitboard knightSquares, Bitboard friendlyPawnAttacks) {
    int count = 0;

    // Iterate over the knight positions using a while loop and bit manipulation
    while (knightSquares) {
        // Isolate the least significant bit that is set (a knight's position)
        Bitboard ls1b = knightSquares & -knightSquares;

        // Check if this knight is on a weak square and defended by a pawn
        if ((ls1b & weakSquares) && (ls1b & friendlyPawnAttacks)) {
            count++;
        }

        // Remove this knight from the set of knights to process the next one
        knightSquares &= ~ls1b;
    }

    return count;
}

// bishop mobility is the number of squares the bishop can move to
short bishopMobility(Bitboard bAttacks){
    return builtin::popcount(bAttacks);
}


// 1 if white has bishop pair and black doesn't, 
// -1 if black has bishop pair and white doesn't
// 0 if neither side has bishop pair
short bishopPair(Bitboard bishops){
    if (builtin::popcount(bishops) >= 2){
        return 1;
    }
    return 0;
}

short rookAttackKingFile(Color color, Bitboard rooks, Bitboard king) {
    Square kingSquare = bitboard_to_square(king);
    int kingFile = file_of(kingSquare);

    short count = 0;
    Bitboard fileMask = fileBitboard(kingFile); // Assuming you have a function to generate a bitmask for a file

    // Check if any rooks are on the same file as the king
    Bitboard attackingRooks = rooks & fileMask;
    while (attackingRooks) {
        builtin::poplsb(attackingRooks); // Remove the found rook from the attackingRooks bitboard
        count++;
    }

    return count;
}



short rookAttackKingAdjFile(Color color, Bitboard rooks, Bitboard king) {
    Square kingSquare = bitboard_to_square(king);
    int kingFile = file_of(kingSquare);

    short count = 0;
    // Create masks for the files adjacent to the king's file
    Bitboard adjFilesMask = 0ULL;
    if (kingFile > 0) adjFilesMask |= fileBitboard(kingFile - 1); // Left adjacent file
    if (kingFile < 7) adjFilesMask |= fileBitboard(kingFile + 1); // Right adjacent file

    // Check if any rooks are on the adjacent files
    Bitboard attackingRooks = rooks & adjFilesMask;
    while (attackingRooks) {
        builtin::poplsb(attackingRooks); // Remove the found rook from the attackingRooks bitboard
        count++;
    }

    return count;
}


short rookSeventhRank(Color color, Bitboard rooks) {
    
    // Define masks for the 7th rank for white and the 2nd rank for black
    Bitboard seventhRankMask = color == Color::WHITE ? rankBitboard(6) : rankBitboard(1);

    // Apply the mask to find rooks on the correct rank
    Bitboard rooksOnSeventh = rooks & seventhRankMask;

    // Count and return the number of rooks on the 7th rank (for white) or 2nd rank (for black)
    return builtin::popcount(rooksOnSeventh);
}


short rookConnected(Color color, Bitboard rooks, Bitboard allPieces) {

    if (builtin::popcount(rooks) < 2) {
        return 0;
    }

    // Iterate through all pairs of rooks (in a bitboard, this means we need to isolate each rook)
    while (rooks) {
        Square rook1 = builtin::poplsb(rooks); // Isolate and remove the first rook
        Bitboard remainingRooks = rooks; // Copy of the remaining rooks after the first has been removed
        while (remainingRooks) {
            Square rook2 = builtin::poplsb(remainingRooks); // Isolate and remove the next rook

            // Check if they are on the same rank or file
            if ((rank_of(rook1) == rank_of(rook2)) || (file_of(rook1) == file_of(rook2))) {
                // Generate a bitboard with all squares between the two rooks, including themselves
                Bitboard inBetweenB = inBetween(rook1, rook2) | (1ULL << rook1) | (1ULL << rook2);
                
                // If the intersection with all pieces is just the rooks, they are connected
                if (((inBetweenB & allPieces) == ((1ULL << rook1) | (1ULL << rook2)))) {
                    return 1;
                }
            }
        }
    }

    return 0;
}


short rookMobility(Bitboard rookAttacks){
    return builtin::popcount(rookAttacks);
}

// we only care about file based mobility
short rookBehindPassedPawn(Color color, Bitboard rooks, Bitboard passedPawns) {

    short count = 0;

    // Iterate over each rook in the bitboard
    while (rooks) {
        int rookSquare = builtin::poplsb(rooks); // This function removes the least significant bit and returns its index
        int rookFile = file_of(Square(rookSquare));
        int rookRank = rank_of(Square(rookSquare));

        Bitboard filePawns = passedPawns & fileBitboard(rookFile); // Mask for passed pawns on the same file as the rook

        // Iterate over each pawn in the filtered bitboard
        while (filePawns) {
            int pawnSquare = builtin::poplsb(filePawns); // Adjust filePawns in the process
            int pawnRank = rank_of(Square(pawnSquare));

            // Determine if the rook is behind the pawn from its perspective
            if ((color == Color::WHITE && rookRank < pawnRank) || (color == Color::BLACK && rookRank > pawnRank)) {
                count++;
            }
        }
    }

    return count;
}




short rookOpenFile(Color color, Bitboard rooks, Bitboard allPawns) {
    short count = 0;

    // Iterate over each rook in the bitboard
    Bitboard rooksCopy = rooks; // Make a copy to preserve original bitboard
    while (rooksCopy) {
        int rookSquare = builtin::poplsb(rooksCopy); // This function also modifies rooksCopy
        int rookFile = file_of(Square(rookSquare));
        Bitboard fileMask = fileBitboard(rookFile); // Assuming fileBB[] is an array of bitboards for each file

        // Check if there are no pawns on the rook's file
        if ((allPawns & fileMask) == 0) {
            count++;
        }
    }

    return count;
}


short rookSemiOpenFile(Color color, Bitboard rooks, Bitboard friendlyPawns, Bitboard enemyPawns) {
    short count = 0;

    Bitboard rooksCopy = rooks; // Make a copy to preserve the original bitboard
    while (rooksCopy) {
        int rookSquare = builtin::poplsb(rooksCopy); // Adjust rooksCopy and get the next rook square
        int rookFile = file_of(Square(rookSquare));
        Bitboard fileMask = fileBitboard(rookFile); // Assuming fileBB[] is an array of bitboards for each file

        // Check if there are no friendly pawns and there are enemy pawns on the rook's file
        if ((friendlyPawns & fileMask) == 0 && (enemyPawns & fileMask) != 0) {
            count++;
        }
    }

    return count;
}


short rookAtckWeakPawnOpenColumn(Color color, Bitboard rooks, Bitboard weakPawns) {
    
    short count = 0;

    Bitboard rooksCopy = rooks; // Make a copy to preserve the original bitboard
    while (rooksCopy) {
        int rookSquare = builtin::poplsb(rooksCopy); // Adjust rooksCopy and get the next rook square
        int rookFile = file_of(Square(rookSquare));
        Bitboard fileMask = fileBitboard(rookFile); // Assuming fileBB[] is an array of bitboards for each file

        // If there's a weak pawn on the same file as the rook, increase the count
        if (fileMask & weakPawns) {
            count++;
        }
    }

    return count;
}


short kingFriendlyPawn(Bitboard friendlyPawns, Bitboard king) {
    
    Square kingSquare = bitboard_to_square(king);
    int kingRank = rank_of(kingSquare);
    int kingFile = file_of(kingSquare);
    short count = 0;
    
    Bitboard pawnsCopy = friendlyPawns; // Make a copy to preserve the original bitboard
    int pawnCount = 0; // Track the number of relevant pawns

    while (pawnsCopy) {
        int pawnSquare = builtin::poplsb(pawnsCopy); // Adjust pawnsCopy and get the next pawn square
        int pawnRank = rank_of(Square(pawnSquare));
        int pawnFile = file_of(Square(pawnSquare)); // these rank and file functions are redundant tbh

        // Only consider pawns on the same or adjacent file
        if (abs(pawnFile - kingFile) <= 1) {
            pawnCount++;
            int distance = abs(pawnRank - kingRank) + abs(pawnFile - kingFile) + 1;
            if (distance > 6) {
                count += 6; // Cap the effect at a maximum distance
            } else {
                count += 6 - distance; // Closer pawns add less to the count
            }
        }
    }

    return count;
}

// the higher the score the better, need to add a limit
// if there is not pawns within a certain radius, it should not affect us


short kingNoEnemyPawnNear(Bitboard enemyPawns, Bitboard king) {
    Square kingSquare = bitboard_to_square(king);
    int kingRank = rank_of(kingSquare);
    int kingFile = file_of(kingSquare);
    short count = 0;

    Bitboard pawnsCopy = enemyPawns; // Make a copy to preserve the original bitboard

    while (pawnsCopy) {
        int pawnSquare = builtin::poplsb(pawnsCopy); // Adjust pawnsCopy and get the next pawn square
        int pawnRank = rank_of(Square(pawnSquare));
        int pawnFile = file_of(Square(pawnSquare));

        // Consider only pawns within 3 moves and on the same or adjacent file
        if (abs(pawnFile - kingFile) <= 1 && abs(pawnRank - kingRank) <= 3) {
            int distance = abs(pawnRank - kingRank) + abs(pawnFile - kingFile) + 1;
            count += distance; // Closer pawns increase the count
        }
    }

    // Invert the score since closer enemy pawns are negative for the king's side
    return -count;
}


// this uses a feature known as king tropism, where
// the score is weighted by piece value and proximity to the king it is attacking. 
// we already account for pawns in the previous round 
// still this is rather crude and should be improved upon
float kingPressureScore(Bitboard king, Bitboard enemyPieces, Board board){
    float score = 0;
    Square kingSquare = bitboard_to_square(king);
    int kingRank = rank_of(kingSquare);
    int kingFile = file_of(kingSquare);
    while (enemyPieces){
        Square enemySquare = chess::builtin::poplsb(enemyPieces);
        int enemyRank = rank_of(enemySquare);
        int enemyFile = file_of(enemySquare);
        // distance is the manhattan distance
        int distance =abs(enemyRank - kingRank) +  abs(enemyFile - kingFile);
        PieceType pieceType = board.at<PieceType>(enemySquare);
        if (pieceType == PieceType::KNIGHT){
            score += 3.0 / distance;
        }
        else if (pieceType == PieceType::BISHOP){
            score += 3.0 / distance;
        }
        else if (pieceType == PieceType::ROOK){
            score += 5.0 / distance;
        }
        else if (pieceType == PieceType::QUEEN){
            score += 9.0 / distance;
        }
    }
    return score;
}






