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
#pragma once

using namespace chess;
using namespace std;


//helper functions
inline constexpr Color opposite_color(Color color) {
    return (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
}

inline constexpr Bitboard square_to_bitmap(Square sq) {
    return Bitboard(1) << sq; // Shifts the 1 to the position of the square
}

inline constexpr int rank_of(Square sq) {
    return static_cast<int>(sq) / 8; // Divide the square index by 8 to get the rank
}

inline constexpr int file_of(Square sq) {
    return static_cast<int>(sq) % 8; // Modulo the square index by 8 to get the file
}

// Helper function to convert file index to string representation (a-h)
inline  std::string fileToString(int file) {
    // Assuming file is 0-7 for a-h
    return std::string(1, 'a' + file);
}

// Helper function to create a bitboard for a specific file
inline Bitboard fileBitboard(int file) {
    Bitboard mask = 0x0101010101010101; // Bitboard with the a-file set
    return mask << file; // Shift to the appropriate file
}

inline constexpr Bitboard rankBitboard(int rank) {
    // Bitboard with the 1s on the 1st rank
    Bitboard mask = 0xFF;
    return mask << (rank * 8); // Shift to the appropriate rank
}

inline constexpr Square int_to_square(int sq) {
    return static_cast<Square>(sq);
}

inline constexpr Square bitboard_to_square(const Bitboard& bitboard) {
    return int_to_square(__builtin_ctzll(bitboard));
}

inline constexpr Bitboard inBetween(Square sq1, Square sq2) {
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




inline constexpr Bitboard blocking_squares(Square sq, Color col, const Bitboard& enemyPawns) {
    int rank = rank_of(sq), file = file_of(sq);
    Bitboard fileMask = 0x0101010101010101ULL << file; // Vertical mask for the current file

    // Create masks for adjacent files if they exist
    Bitboard leftFileMask = file > 0 ? (fileMask >> 1) : 0;
    Bitboard rightFileMask = file < 7 ? (fileMask << 1) : 0;

    // Combine masks for the current and adjacent files
    Bitboard combinedMask = fileMask | leftFileMask | rightFileMask;

    // Calculate the range mask based on the color and position
    if (col == Color::WHITE) {
        Bitboard northMask = (0xFFFFFFFFFFFFFFFFULL << ((rank + 1) * 8));
        return northMask & combinedMask;
    } else {
        Bitboard southMask = (0xFFFFFFFFFFFFFFFFULL >> ((8 - rank) * 8));
        return southMask & combinedMask;
    }
}


inline constexpr Bitboard shiftNorth(const Bitboard& b) { return b << 8; }
inline constexpr Bitboard shiftSouth(const Bitboard& b) { return b >> 8; }
inline constexpr Bitboard shiftEast(const Bitboard& b) { return (b & 0xFEFEFEFEFEFEFEFEULL) << 1; } // Prevent wraparound
inline constexpr Bitboard shiftWest(const Bitboard& b) { return (b & 0x7F7F7F7F7F7F7F7FULL) >> 1; }
// Function to calculate eastward attacks for white pawns

Bitboard wPawnEastAttacks(const Bitboard& wpawns) {
    return (wpawns << 9) & ~0x0101010101010101; // Avoid wraparound from H to A file
}

// Function to calculate westward attacks for white pawns
inline constexpr Bitboard wPawnWestAttacks(const Bitboard& wpawns) {
    return (wpawns << 7) & ~0x8080808080808080; // Avoid wraparound from A to H file
}

// Function to calculate eastward attack spans for white pawns
Bitboard wEastAttackFrontSpans(const Bitboard& wpawns) {
    Bitboard spans = 0;
    Bitboard attacks = wPawnEastAttacks(wpawns);
    while (attacks) {
        spans |= attacks;
        attacks <<= 8; // Move one rank up
    }
    return spans;
}

// Function to calculate westward attack spans for white pawns
Bitboard wWestAttackFrontSpans(const Bitboard& wpawns) {
    Bitboard spans = 0;
    Bitboard attacks = wPawnWestAttacks(wpawns);
    while (attacks) {
        spans |= attacks;
        attacks <<= 8; // Move one rank up
    }
    return spans;
}

// Function to calculate eastward attacks for black pawns
Bitboard bPawnEastAttacks(const Bitboard& bpawns) {
    return (bpawns >> 7) & ~0x0101010101010101; // Avoid wraparound from H to A file
}

// Function to calculate westward attacks for black pawns
Bitboard bPawnWestAttacks(const Bitboard& bpawns) {
    return (bpawns >> 9) & ~0x8080808080808080; // Avoid wraparound from A to H file
}

// Assuming the functions for bPawnEastAttacks and bPawnWestAttacks are already defined as:
// bPawnEastAttacks: Calculates eastward attacks for black pawns
// bPawnWestAttacks: Calculates westward attacks for black pawns

// Function to calculate eastward attack spans for black pawns
Bitboard bEastAttackFrontSpans(const Bitboard& bpawns) {
    Bitboard spans = 0;
    Bitboard attacks = bPawnEastAttacks(bpawns);
    while (attacks) {
        spans |= attacks;
        attacks >>= 8; // Move one rank down
    }
    return spans;
}

// Function to calculate westward attack spans for black pawns
Bitboard bWestAttackFrontSpans(const Bitboard& bpawns) {
    Bitboard spans = 0;
    Bitboard attacks = bPawnWestAttacks(bpawns);
    while (attacks) {
        spans |= attacks;
        attacks >>= 8; // Move one rank down
    }
    return spans;
}

Bitboard detectPassedPawns(Color col, Bitboard ownPawns, const Bitboard& enemyPawns) {
    Bitboard passedPawns = 0;

    while (ownPawns) {
        Bitboard pawn = ownPawns & -ownPawns; // Isolate the least significant bit representing a pawn
        Bitboard blockSquares = blocking_squares(bitboard_to_square(pawn), col, enemyPawns);

        // Check if any enemy pawn is in the blocking squares
        if (!(blockSquares & enemyPawns)) {
            passedPawns |= pawn;
        }

        ownPawns &= ownPawns - 1; // Move to the next pawn
    }

    return passedPawns;
}



int detectDoubledPawns(Color color, const Bitboard& wpawns, const Bitboard& bpawns) {
    const auto& pawns = (color == Color::WHITE) ? wpawns : bpawns;
    Bitboard filaA = Bitboard(File::FILE_A);
    Bitboard filaB = Bitboard(File::FILE_B);
    Bitboard filaC = Bitboard(File::FILE_C);
    Bitboard filaD = Bitboard(File::FILE_D);
    Bitboard filaE = Bitboard(File::FILE_E);
    Bitboard filaF = Bitboard(File::FILE_F);
    Bitboard filaG = Bitboard(File::FILE_G);
    Bitboard filaH = Bitboard(File::FILE_H);
    int doubledPawns = 0;

    const Bitboard fileMasks[] = {filaA, filaB, filaC, filaD, filaE, filaF, filaG, filaH};
    for (const auto& fileMask : fileMasks) {
        int count = builtin::popcount(pawns & fileMask);
        if (count > 1) {
            doubledPawns += count - 1;
        }
    }
    return doubledPawns;
}


Bitboard detectIsolatedPawns(Bitboard pawns) {
    Bitboard isolated = 0;
    const auto allPawns = pawns;

    while (pawns != 0) {
        const auto sq = builtin::poplsb(pawns);
        int file = file_of(sq);
        Bitboard leftFileMask = file > 0 ? fileBitboard(file - 1) : 0;
        Bitboard rightFileMask = file < 7 ? fileBitboard(file + 1) : 0;

        if ((allPawns & (leftFileMask | rightFileMask)) == 0) {
            isolated |= Bitboard(1) << sq;
        }
    }

    return isolated;
}


Bitboard detectCentralPawns(const Bitboard& pawns) {
    Bitboard centerMask = 0x0000001818000000; // Mask for the central 4 squares (e4, d4, e5, d5)
    return pawns & centerMask; // Directly return the intersection of pawns and the central squares
}

// Assuming `enemyAttacks` correctly represents squares attacked by all enemy pieces,
// not just enemy pawns. If it only represents enemy pawn attacks, adjust accordingly.
Bitboard wBackward(const Bitboard& wpawns, const Bitboard& bpawns) {
    Bitboard stops = wpawns << 8;
    Bitboard wAttackSpans = wEastAttackFrontSpans(wpawns) | wWestAttackFrontSpans(wpawns);
    Bitboard bAttacks = bPawnEastAttacks(bpawns) | bPawnWestAttacks(bpawns);
    return (stops & bAttacks & ~wAttackSpans) >> 8;
}

Bitboard bBackward(const Bitboard& bpawns, const Bitboard& wpawns) {
    Bitboard stops = bpawns >> 8;
    Bitboard bAttackSpans = bEastAttackFrontSpans(bpawns) | bWestAttackFrontSpans(bpawns);
    Bitboard wAttacks = wPawnEastAttacks(wpawns) | wPawnWestAttacks(wpawns);
    return (stops & wAttacks & ~bAttackSpans) << 8;
}



Bitboard detectWeakSquares(Color color, Bitboard pawns) {
    Bitboard potentialCoverage = 0; // Initially, no squares are covered.

    while (pawns) {
        Bitboard pawnPos = pawns & -pawns; // Isolate the least significant bit representing a pawn
        pawns &= pawns - 1; // Remove this pawn from consideration

        // Project pawn attacks upwards or downwards through the entire board
        // For White, project attacks to the left and right diagonally upwards
        // For Black, project attacks to the left and right diagonally downwards
        if (color == Color::WHITE) {
            Bitboard attacksLeft = (pawnPos & ~fileBitboard(0)) << 9; // Project left attacks upwards
            Bitboard attacksRight = (pawnPos & ~fileBitboard(7)) << 7; // Project right attacks upwards
            // Fill upwards
            while (attacksLeft) {
                potentialCoverage |= attacksLeft;
                attacksLeft <<= 8; // Move straight up
            }
            while (attacksRight) {
                potentialCoverage |= attacksRight;
                attacksRight <<= 8; // Move straight up
            }
        } else {
            Bitboard attacksLeft = (pawnPos & ~fileBitboard(0)) >> 7; // Project left attacks downwards
            Bitboard attacksRight = (pawnPos & ~fileBitboard(7)) >> 9; // Project right attacks downwards
            // Fill downwards
            while (attacksLeft) {
                potentialCoverage |= attacksLeft;
                attacksLeft >>= 8; // Move straight down
            }
            while (attacksRight) {
                potentialCoverage |= attacksRight;
                attacksRight >>= 8; // Move straight down
            }
        }
    }

    // Define the target area for weak square analysis
    // Assuming we're interested in the middle of the board for both sides
    Bitboard targetArea = rankBitboard(2) | rankBitboard(3) | rankBitboard(4) | rankBitboard(5);

    // Weak squares are those in the target area not covered by potential pawn moves
    Bitboard weakSquares = targetArea & ~potentialCoverage;

    return weakSquares;
}









int ruleOfTheSquare(Color color, Bitboard passedPawns, const Bitboard& king) {
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

        
        bool canCatchUp = true;
        
        if(color == Color::WHITE && passedPawnRank < kingRank){
            canCatchUp = false;
        }
        if(color == Color::BLACK && passedPawnRank > kingRank){
            canCatchUp = false;
        }
        if(distanceToPromotion < abs(kingRank - passedPawnRank)){
            canCatchUp = false;
        }
        // If the king can reach the pawn's promotion path before or as it promotes, count it
        
        if(canCatchUp){
            count++;
        }
        passedPawns &= passedPawns - 1; // Remove this pawn from the set
    }
    return count;
}


// weak squares are the opposite of the color
int knightOutposts(const Bitboard& weakSquares, const Bitboard& knightSquares, const Bitboard& friendlyPawnAttacks) {
    Bitboard validOutposts = weakSquares & friendlyPawnAttacks & knightSquares; // Outposts are weak squares guarded by pawns
    validOutposts &= ~fileBitboard(0) & ~fileBitboard(7) & ~rankBitboard(1) & ~rankBitboard(6);

    
    return builtin::popcount(validOutposts);
}

// more mobility features
int knightMobility(const Bitboard& knightAttacks){
    return chess::builtin::popcount(knightAttacks);
}

// more mobility features
int queenMobility(const Bitboard& queenAttacks){
    return chess::builtin::popcount(queenAttacks);
}

// bishop mobility is the number of squares the bishop can move to
int bishopMobility(const Bitboard& bAttacks){
    return chess::builtin::popcount(bAttacks);
}


// 1 if white has bishop pair and black doesn't, 
// -1 if black has bishop pair and white doesn't
// 0 if neither side has bishop pair
int bishopPair(const Bitboard& bishops){
    if (builtin::popcount(bishops) >= 2){
        return 1;
    }
    return 0;
}

int rookAttackKingFile(Color color, const Bitboard& rooks, const Bitboard& king) {
    Square kingSquare = bitboard_to_square(king);
    int kingFile = file_of(kingSquare);

    int count = 0;
    Bitboard fileMask = fileBitboard(kingFile); // Assuming you have a function to generate a bitmask for a file

    // Check if any rooks are on the same file as the king
    Bitboard attackingRooks = rooks & fileMask;
    while (attackingRooks) {
        builtin::poplsb(attackingRooks); // Remove the found rook from the attackingRooks bitboard
        count++;
    }

    return count;
}



int rookAttackKingAdjFile(Color color, const Bitboard& rooks, const Bitboard& king) {
    Square kingSquare = bitboard_to_square(king);
    int kingFile = file_of(kingSquare);

    int count = 0;
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


int rookSeventhRank(Color color, const Bitboard& rooks) {
    
    // Define masks for the 7th rank for white and the 2nd rank for black
    Bitboard seventhRankMask = color == Color::WHITE ? rankBitboard(6) : rankBitboard(1);

    // Apply the mask to find rooks on the correct rank
    Bitboard rooksOnSeventh = rooks & seventhRankMask;

    // Count and return the number of rooks on the 7th rank (for white) or 2nd rank (for black)
    return builtin::popcount(rooksOnSeventh);
}


int rookConnected(Color color, Bitboard rooks, const Bitboard& allPieces) {

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


int rookMobility(const Bitboard& rookAttacks){
    return chess::builtin::popcount(rookAttacks);
}

// we only care about file based mobility
int rookBehindPassedPawn(Color color, Bitboard rooks, const Bitboard& passedPawns) {

    int count = 0;

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




int rookOpenFile(Color color, Bitboard rooks, const Bitboard &allPawns) {
    int count = 0;
    while (rooks) {
        int rookSquare = builtin::poplsb(rooks); // This function also modifies rooksCopy
        int rookFile = file_of(Square(rookSquare));
        Bitboard fileMask = fileBitboard(rookFile); // Assuming fileBB[] is an array of bitboards for each file

        // Check if there are no pawns on the rook's file
        if ((allPawns & fileMask) == 0) {
            count++;
        }
    }

    return count;
}


int rookSemiOpenFile(Color color, Bitboard rooks, const Bitboard& friendlyPawns, const Bitboard& enemyPawns) {
    int count = 0;
    while (rooks) {
        int rookSquare = builtin::poplsb(rooks); // Adjust rooksCopy and get the next rook square
        int rookFile = file_of(Square(rookSquare));
        Bitboard fileMask = fileBitboard(rookFile); // Assuming fileBB[] is an array of bitboards for each file

        // Check if there are no friendly pawns and there are enemy pawns on the rook's file
        if ((friendlyPawns & fileMask) == 0 && (enemyPawns & fileMask) != 0) {
            count++;
        }
    }

    return count;
}


int rookAtckWeakPawnOpenColumn(Color color, Bitboard rooks, const Bitboard& weakPawns) {
    
    int count = 0;
    while (rooks) {
        int rookSquare = builtin::poplsb(rooks); // Adjust rooksCopy and get the next rook square
        int rookFile = file_of(Square(rookSquare));
        Bitboard fileMask = fileBitboard(rookFile); // Assuming fileBB[] is an array of bitboards for each file

        // If there's a weak pawn on the same file as the rook, increase the count
        if (fileMask & weakPawns) {
            count++;
        }
    }

    return count;
}


int kingFriendlyPawn(Bitboard friendlyPawns, const Bitboard& king) {
    
    Square kingSquare = bitboard_to_square(king);
    int kingRank = rank_of(kingSquare);
    int kingFile = file_of(kingSquare);
    int count = 0;
    
    int pawnCount = 0; // Track the number of relevant pawns

    while (friendlyPawns) {
        int pawnSquare = builtin::poplsb(friendlyPawns); // Adjust pawnsCopy and get the next pawn square
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


int kingNoEnemyPawnNear(Bitboard enemyPawns, const Bitboard& king) {
    Square kingSquare = bitboard_to_square(king);
    int kingRank = rank_of(kingSquare);
    int kingFile = file_of(kingSquare);
    int count = 0;

    while (enemyPawns) {
        int pawnSquare = builtin::poplsb(enemyPawns); // Adjust pawnsCopy and get the next pawn square
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

// stuff for king pressure
// Constants for edge masks to prevent wrapping around the board
const Bitboard NOT_A_FILE = 0xfefefefefefefefe; // ~0x0101010101010101
const Bitboard NOT_H_FILE = 0x7f7f7f7f7f7f7f7f; // ~0x8080808080808080


constexpr Bitboard shiftNE( const Bitboard& bitboard) {
    return (bitboard << 9) & NOT_A_FILE;
}

constexpr Bitboard shiftNW(const Bitboard& bitboard) {
    return (bitboard << 7) & NOT_H_FILE;
}

constexpr Bitboard shiftSE(const Bitboard& bitboard) {
    return (bitboard >> 7) & NOT_A_FILE;
}

constexpr Bitboard shiftSW(const Bitboard& bitboard) {
    return (bitboard >> 9) & NOT_H_FILE;
}

Bitboard expandToAdjacentSquares(const Bitboard& bitboard) {
    Bitboard expanded = bitboard;
    expanded |= shiftNorth(bitboard) | shiftSouth(bitboard);
    expanded |= shiftEast(bitboard) | shiftWest(bitboard);
    expanded |= shiftNE(bitboard) | shiftNW(bitboard);
    expanded |= shiftSE(bitboard) | shiftSW(bitboard);
    return expanded;
}


Bitboard calculateKingsZone(const Bitboard& kingPosition, Color color) {
    Bitboard kingZone = 0;

    // Expand to include all adjacent squares in the king's zone.
    kingZone |= expandToAdjacentSquares(kingPosition);
    kingZone |= shiftNorth(kingZone) | shiftSouth(kingZone);

    return kingZone;
}


int kingPressureScore(const Bitboard& king, const Bitboard& enemyKnights, const Bitboard& enemyBishops, const Bitboard& enemyRooks, const Bitboard& enemyQueens, Color color, const Board& board) {
    int score = 0;
    Bitboard kingZone = calculateKingsZone(king, color); // Assume a function similar to calculateKingsZone but more nuanced

    // Factor in different piece types with tailored evaluations
    score += builtin::popcount(kingZone & enemyKnights) * 1;
    score += builtin::popcount(kingZone & enemyBishops) * 1;
    score += builtin::popcount(kingZone & enemyRooks) * 3;
    score += builtin::popcount(kingZone & enemyQueens) * 5;

    return score;
}







