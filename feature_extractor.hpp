//This file is where anything having to do with raw feature extraction is done. 
// We have a FeatureExtractor Class, which takes in a Board object, and then extracts all the features from the current position
// Built in is also a Feature tester class, which contians a list of FEN strings, and their associated expected features
// we extract features from each FEN, and see if they match up with the expected test results. If not, we can delve further to try 
// and make the feature extraction bullet-proof. Optimization will probably be a secondary concern
// optimization possibilities:
    // detecting for lazy evaluation
    // not recalculating attacks, and certain bitboards THIS IS HUGE, right now I re-calculate anything I need within each function

// important distinction: These are the raw features, not the features weighted by the evaluation function, that is a whole different story. 

#include "chess.hpp"
#include "features.hpp" 

using namespace chess;
using namespace std;
// command to run in terminal: g++ -std=c++17 -O3 -march=native -o example chess.cpp


class FeatureExtractor {
private:
    Features features;
    const Board& board;

    // Cache for commonly used bitboards and calculations
    Bitboard wpawns, bpawns, allPawns;
    Bitboard wAttacks, bAttacks;

    Bitboard wWSquares, wBSquares; // store the weak squares for black and white
    
    Bitboard wWpawns, bWpawns; // store the weak pawns for black and white

    vector<Square> wPassedPawns, bPassedPawns; // store the passed pawns for black and white

    vector<Square> wPawnSquares, bPawnSquares; // Store pawn squares for both colors for quick access
    vector<Square> wKnightSquares, bKnightSquares; // store knight squares
    vector<Square> wRookSquares, bRookSquares; // store rook squares


    Square whiteKingSquare, blackKingSquare; // store king squares


    // Initialize caches (call this in the extract method)
    void initCaches() {
        wpawns = board.pieces(PieceType::PAWN, Color::WHITE);
        bpawns = board.pieces(PieceType::PAWN, Color::BLACK);
        allPawns = wpawns | bpawns;
        wAttacks = wPawnAttacks(wpawns);
        bAttacks = bPawnAttacks(bpawns);

        wPawnSquares = findPieceSquares(PieceType::PAWN, Color::WHITE);
        bPawnSquares = findPieceSquares(PieceType::PAWN, Color::BLACK);

        wKnightSquares = findPieceSquares(PieceType::KNIGHT, Color::WHITE);
        bKnightSquares = findPieceSquares(PieceType::KNIGHT, Color::BLACK);

        wRookSquares = findPieceSquares(PieceType::ROOK, Color::WHITE);
        bRookSquares = findPieceSquares(PieceType::ROOK, Color::BLACK);

        whiteKingSquare = board.kingSq(Color::WHITE); 
        blackKingSquare = board.kingSq(Color::BLACK);
    }

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

 // Example of a modified feature extraction function using cached data
    vector<Square> detectPassedPawns(Color col) {
        vector<Square>& pawnSquares = col == Color::WHITE ? wPawnSquares : bPawnSquares;
        Bitboard enemyPawns = col == Color::WHITE ? bpawns : wpawns;

        vector<Square> passedPawns;
        for (Square pawnSquare : pawnSquares) {
            Bitboard blockingSquares = blocking_squares(pawnSquare, col, enemyPawns);
            if ((blockingSquares & enemyPawns) == 0) {
                passedPawns.push_back(pawnSquare);
            }
        }
        // store results for later use
        if (col == Color::WHITE){
            wPassedPawns = passedPawns;
        }
        else{
            bPassedPawns = passedPawns;
        }
        return passedPawns;
    }



int detectDoubledPawns(Color color) {
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

Bitboard detectWeakPawns(Color color) {
    const Bitboard pawns = (color == Color::WHITE) ? wpawns : bpawns;
    const Bitboard enemyPawns = (color == Color::WHITE) ? bpawns : wpawns;
    Bitboard backwardsPawns = 0;

    // Masks for pawn attacks, considering only pawns that can support each other
    Bitboard leftSupport = (color == Color::WHITE) ? (pawns << 1) & ~fileBitboard(0) : (pawns >> 1) & ~fileBitboard(0);
    Bitboard rightSupport = (color == Color::WHITE) ? (pawns >> 1) & ~fileBitboard(7) : (pawns << 1) & ~fileBitboard(7);

    // Iterate through each pawn to check if it's a backwards pawn
    for (int sq = SQ_A1; sq <= SQ_H8; ++sq) {
        Square square = static_cast<Square>(sq);
        if (!(square_to_bitmap(square) & pawns)) continue; // Skip squares without a pawn of the given color

        // Determine if the pawn lacks support from adjacent files
        bool hasSupport = (square_to_bitmap(square) & (leftSupport | rightSupport));
        if (!hasSupport) {
            // Check if the pawn is blocked by an enemy pawn directly ahead, making it potentially backwards
            Square inFrontSq = (color == Color::WHITE) ? Square(sq + 8) : Square(sq - 8);
            if (inFrontSq >= SQ_A1 && inFrontSq <= SQ_H8 && !(square_to_bitmap(inFrontSq) & enemyPawns)) {
                backwardsPawns |= square_to_bitmap(square);
            }
        }
    }
    // store results for later use
    if (color == Color::WHITE){
        wWpawns = backwardsPawns;
    }
    else{
        bWpawns = backwardsPawns;
    }

    return backwardsPawns;
}

vector<Square> detectIsolatedPawns(Color color) {
    const auto& pawnSquares = (color == Color::WHITE) ? wPawnSquares : bPawnSquares;
    const auto& pawns = (color == Color::WHITE) ? wpawns : bpawns;

    vector<Square> isolatedPawns;
    for (Square pawnSquare : pawnSquares) {
        int file = file_of(pawnSquare);
        Bitboard leftFileMask = file > 0 ? fileBitboard(file - 1) : 0;
        Bitboard rightFileMask = file < 7 ? fileBitboard(file + 1) : 0;
        Bitboard adjacentPawns = pawns & (leftFileMask | rightFileMask);

        if (chess::builtin::popcount(adjacentPawns) == 0) {
            isolatedPawns.push_back(pawnSquare);
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


Bitboard detectWeakSquares(Color color) {
    // Directly utilize the provided cached bitboards
    Bitboard pawns = (color == Color::WHITE) ? wpawns : bpawns;
    Bitboard attacks = (color == Color::WHITE) ? wAttacks : bAttacks;

    // Define target ranks for weak square detection, excluding the edge files (A and H)
    Bitboard targetRanks = (color == Color::WHITE) ? (rankBitboard(4) | rankBitboard(5) | rankBitboard(6)) : (rankBitboard(3) | rankBitboard(4) | rankBitboard(5));
    Bitboard centralFiles = ~(fileBitboard(0) | fileBitboard(7)); // Exclude edge files

    // Squares on the target ranks that are not covered by pawn attacks and excluding edge files
    Bitboard weakSquares = targetRanks & ~attacks & centralFiles;

    if(color == Color::WHITE){
        wWSquares = weakSquares;
    }
    else{
        wBSquares = weakSquares;
    }
    return weakSquares;
}




//returns the number of passed pawns that a king could catch up to in time
int ruleOfTheSquare(Color color) {
    const vector<Square>& passedPawns = detectPassedPawns(opposite_color(color));
    Square kingSquare = (color == Color::WHITE) ? whiteKingSquare : blackKingSquare;

    int count = 0;
    for (Square passedPawnSquare : passedPawns) {
        int passedPawnRank = rank_of(passedPawnSquare);
        int passedPawnFile = file_of(passedPawnSquare);

        // Calculate the "square" of the pawn based on its rank and the direction it advances
        int promotionRank = (color == Color::WHITE) ? 0 : 7; // Rank the pawn needs to reach to promote
        int distanceToPromotion = (color == Color::WHITE) ? 7 - passedPawnRank : passedPawnRank;

        int kingRank = rank_of(kingSquare);
        int kingFile = file_of(kingSquare);

        // Calculate the king's distance to the promotion path of the pawn
        int distanceToStopPromotion = max(abs(kingRank - promotionRank), abs(kingFile - passedPawnFile));

        // If the king can reach the pawn's promotion path before or as it promotes, count it
        if (distanceToStopPromotion <= distanceToPromotion) {
            count++;
        }
    }
    return count;
}


//returns the number of knights on weak squares. This is not quite correct, because an outpost needs to be defended by a pawn, but it is a good approximation
int knightOutposts(Color color) {
    // Access the appropriate cached weak squares and knight positions based on the color
    const Bitboard& weakSquares = (color == Color::WHITE) ? wBSquares : wWSquares; // Use opposite color's weak squares
    const vector<Square>& knightSquares = (color == Color::WHITE) ? wKnightSquares : bKnightSquares;

    int count = 0;

    // Iterate over the knight positions and count how many are on weak squares
    for (Square knightSquare : knightSquares) {
        if (weakSquares & square_to_bitmap(knightSquare)) {
            count++;
        }
    }

    return count;
}

// counts the number of squares a bishop can move to
short bishopMobility(Color color){
    Movelist bishopMoves;
    movegen::legalmoves(bishopMoves, board, PieceGenType::BISHOP);
    return bishopMoves.size();
}

// 1 if white has bishop pair and black doesn't, 
// -1 if black has bishop pair and white doesn't
// 0 if neither side has bishop pair
short bishopPair(){
    short count = 0;
    vector<Square> wbishops = findPieceSquares(PieceType::BISHOP, Color::WHITE);
    vector<Square> bbishops = findPieceSquares(PieceType::BISHOP, Color::BLACK);

    if (wbishops.size() >= 2){
        count ++;
    }
    if(bbishops.size() >= 2){
        count --;
    }
    return count;
}

short rookAttackKingFile(Color color) {
    // Use the cached rook positions based on the color
    const vector<Square>& rooks = (color == Color::WHITE) ? wRookSquares : bRookSquares;
    // Use the cached king square for the opposite color
    Square kingSquare = (color == Color::WHITE) ? blackKingSquare : whiteKingSquare;
    int kingFile = file_of(kingSquare);
    
    short count = 0;
    // Iterate over the cached rook positions to count how many are on the same file as the king
    for (Square rookSquare : rooks) {
        if (file_of(rookSquare) == kingFile) {
            count++;
        }
    }

    return count;
}


short rookAttackKingAdjFile(Color color){
    const vector<Square>& rooks = (color == Color::WHITE) ? wRookSquares : bRookSquares;
    Square kingSquare = (color == Color::WHITE) ? blackKingSquare : whiteKingSquare;
    int kingFile = file_of(kingSquare);
    short count = 0;
    for (Square rookSquare : rooks) {
        int rookFile = file_of(rookSquare);
        if (abs(rookFile - kingFile) == 1){
            count ++;
        }
    }
    return count;
}

short rookSeventhRank(Color color){
    const vector<Square>& rooks = (color == Color::WHITE) ? wRookSquares : bRookSquares;
    short count = 0;
    for (Square rookSquare : rooks) {
        int rookRank = rank_of(rookSquare);
        if (color == Color::WHITE){
            if (rookRank == 6){
                count ++;
            }
        }
        else{
            if (rookRank == 1){
                count ++;
            }
        }
    }
    return count;
}

// function that returns if the rooks could capture each other, assuming they weren't the same color
// assumption:: if we have three or more rooks, who cares if they are connected, we are probably winning
short rookConnected(Color color){
    const vector<Square>& rooks = (color == Color::WHITE) ? wRookSquares : bRookSquares;
    if(rooks.size() < 2){
        return 0;
    }
    Square rook1 = rooks[0];
    Square rook2 = rooks[1]; 
    int rook1Rank = rank_of(rook1);
    int rook2Rank = rank_of(rook2);
    int rook1File = file_of(rook1);
    int rook2File = file_of(rook2);
    if (rook1Rank != rook2Rank && rook1File != rook2File){
        return 0;
    }
    if (rook1Rank == rook2Rank){
        int minFile = min(rook1File, rook2File);
        int maxFile = max(rook1File, rook2File);
        for (int i = minFile + 1; i < maxFile; i++){
            if (board.at<PieceType>(Square(rook1Rank * 8 + i)) != PieceType::NONE){
                return 0;
            }
        }
        return 1;
    }
    if (rook1File == rook2File){
        int minRank = min(rook1Rank, rook2Rank);
        int maxRank = max(rook1Rank, rook2Rank);
        for (int i = minRank + 1; i < maxRank; i++){
            if (board.at<PieceType>(Square(i * 8 + rook1File)) != PieceType::NONE){
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

short rookMobility(Color color){
    Movelist rookMoves;
    movegen::legalmoves(rookMoves, board, PieceGenType::ROOK);
    return rookMoves.size();
}

short rookBehindPassedPawn(Color color) {
    // Access the cached rook squares based on the color
    const vector<Square>& rooks = (color == Color::WHITE) ? wRookSquares : bRookSquares;
    // Access the cached passed pawn squares for the opposite color
    const vector<Square>& passedPawns = (color == Color::WHITE) ? bPassedPawns : wPassedPawns;

    short count = 0;

    // Iterate over the rooks and passed pawns to count the occurrences
    for (Square rookSquare : rooks) {
        int rookRank = rank_of(rookSquare);
        int rookFile = file_of(rookSquare);

        for (Square passedPawnSquare : passedPawns) {
            int passedPawnRank = rank_of(passedPawnSquare);
            int passedPawnFile = file_of(passedPawnSquare);

            // Check if the rook and the pawn are on the same file
            if (rookFile == passedPawnFile) {
                // For white, the rook must be on a lower rank than the pawn; for black, on a higher rank
                if ((color == Color::WHITE && rookRank < passedPawnRank) || (color == Color::BLACK && rookRank > passedPawnRank)) {
                    count++;
                }
            }
        }
    }

    return count;
}


short rookOpenFile(Color color){
    const vector<Square>& rooks = (color == Color::WHITE) ? wRookSquares : bRookSquares;
    short count = 0;
    for (Square rookSquare : rooks) {
        int rookFile = file_of(rookSquare);
        Bitboard fileMask = fileBitboard(rookFile);
        Bitboard friendlyPawns = board.pieces(PieceType::PAWN, color);
        Bitboard enemyPawns = board.pieces(PieceType::PAWN, opposite_color(color));
        if ((friendlyPawns & fileMask & enemyPawns) == 0){
            count ++;
        }
    }
    return count;
}

short rookSemiOpenFile(Color color){
    const vector<Square>& rooks = (color == Color::WHITE) ? wRookSquares : bRookSquares;
    short count = 0;
    for (int i = 0; i < rooks.size(); i++){
        Square rookSquare = rooks[i];
        int rookFile = file_of(rookSquare);
        Bitboard fileMask = fileBitboard(rookFile);
        Bitboard friendlyPawns = board.pieces(PieceType::PAWN, color);
        Bitboard enemyPawns = board.pieces(PieceType::PAWN, opposite_color(color));
        if ((friendlyPawns & fileMask) == 0 && (enemyPawns & fileMask) != 0){
            count ++;
        }
    }
    return count;
}

short rookAtckWeakPawnOpenColumn(Color color) {
    // Directly access the cached rook positions
    const vector<Square>& rooks = (color == Color::WHITE) ? wRookSquares : bRookSquares;
    // Use the bitboard for weak pawns of the opposite color
    Bitboard weakPawns = (color == Color::WHITE) ? bWpawns : wWpawns;
    
    short count = 0;

    for (Square rookSquare : rooks) {
        int rookFile = file_of(rookSquare);
        Bitboard fileMask = fileBitboard(rookFile);

        // If there's a weak pawn on the same file as the rook, increase the count
        // This check is simplified by directly comparing the file mask with the weak pawns bitboard
        if (fileMask & weakPawns) {
            // Further ensure the column is open, i.e., no pawns of either color present on the rook's file
            Bitboard columnPawns = fileMask & allPawns;
            if (!(columnPawns & ~(square_to_bitmap(rookSquare)))) { // Ensure no other pawns are on the rook's file
                count++;
            }
        }
    }

    return count;
}
// How many and how close are friendly pawns (closeness * numberofpawns / numberofpawns)
//only relevant to pawns on the same or adjacent files, the lower the score the better
// need to add a heav punishment if there is not three pawns, two or even one
short kingFriendlyPawn(Color color){
    vector<Square> friendlyPawns = color == Color::WHITE ? wPawnSquares : bPawnSquares;
    Square kingSquare = color == Color::WHITE ? whiteKingSquare : blackKingSquare;
    int kingRank = rank_of(kingSquare);
    int kingFile = file_of(kingSquare);
    short count = 0;
    for (int i = 0; i < friendlyPawns.size(); i++){
        Square pawnSquare = friendlyPawns[i];

        int pawnRank = rank_of(pawnSquare);
        int pawnFile = file_of(pawnSquare);

        if(abs(pawnFile - kingFile) > 1){
            continue;
        }
        int distance = max(abs(pawnRank - kingRank), abs(pawnFile - kingFile));
        count += distance;
    }
    return count;
}

// the higher the score the better, need to add a limit
// if there is not pawns within a certain radius, it should not affect us
short kingNoEnemyPawnNear(Color color){
    vector<Square> enemyPawns = color == Color::WHITE ? bPawnSquares : wPawnSquares;
    Square kingSquare = color == Color::WHITE ? whiteKingSquare : blackKingSquare;
    int kingRank = rank_of(kingSquare);
    int kingFile = file_of(kingSquare);
    short count = 0;
    for (int i = 0; i < enemyPawns.size(); i++){
        Square pawnSquare = enemyPawns[i];

        int pawnRank = rank_of(pawnSquare);
        int pawnFile = file_of(pawnSquare);

        if(abs(pawnFile - kingFile) > 1){
            continue;
        }
        int distance = max(abs(pawnRank - kingRank), abs(pawnFile - kingFile));
        count += distance;
    }
    return count;
}

// this uses a feature known as king tropism, where
// the score is weighted by piece value and proximity to the king it is attacking. 
// we already account for pawns in the previous round 
// still this is rather crude and should be improved upon
float kingPressureScore(Color color){
    float score = 0;
    Square kingSquare = color == Color::WHITE ? whiteKingSquare : blackKingSquare;
    int kingRank = rank_of(kingSquare);
    int kingFile = file_of(kingSquare);
    Bitboard enemyPieces = board.pieces(PieceType::BISHOP, color) & board.pieces(PieceType::KNIGHT, color) & board.pieces(PieceType::ROOK, color) & board.pieces(PieceType::QUEEN, color);
    while (enemyPieces){
        Square enemySquare = chess::builtin::poplsb(enemyPieces);
        int enemyRank = rank_of(enemySquare);
        int enemyFile = file_of(enemySquare);
        int distance = max(abs(enemyRank - kingRank), abs(enemyFile - kingFile));
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




public:
    FeatureExtractor(const Board& board) : board(board) {
    }

    Features getFeatures() {
        return features;
    }

    void extract() {
        // Extract the features
        features.fen = board.getFen();
        initCaches();
        //this one is a bitboard instead of a vector, might be faster?
        features.weakPawns = __builtin_popcountll(detectWeakPawns(Color::WHITE)) - __builtin_popcountll(detectWeakPawns(Color::BLACK));
        features.doubledPawns = detectDoubledPawns(Color::WHITE) - detectDoubledPawns(Color::BLACK);
        features.isolatedPawns = detectIsolatedPawns(Color::WHITE).size() - detectIsolatedPawns(Color::BLACK).size();
        features.passedPawns = detectPassedPawns(Color::WHITE).size() - detectPassedPawns(Color::BLACK).size();
        features.weakSquares = chess::builtin::popcount(detectWeakSquares(Color::WHITE)) - chess::builtin::popcount(detectWeakSquares(Color::BLACK));
        features.passedPawnEnemyKingSquare = ruleOfTheSquare(Color::WHITE) - ruleOfTheSquare(Color::BLACK);
        features.knightOutposts = knightOutposts(Color::WHITE) - knightOutposts(Color::BLACK);
        features.bishopMobility = bishopMobility(Color::WHITE) - bishopMobility(Color::BLACK);
        features.bishopPair = bishopPair();
        features.rookAttackKingFile = rookAttackKingFile(Color::WHITE) - rookAttackKingFile(Color::BLACK);
        features.rookAttackKingAdjFile = rookAttackKingAdjFile(Color::WHITE) - rookAttackKingAdjFile(Color::BLACK);
        features.rook7thRank = rookSeventhRank(Color::WHITE) - rookSeventhRank(Color::BLACK);
        features.rookConnected = rookConnected(Color::WHITE) - rookConnected(Color::BLACK);
        features.rookMobility = rookMobility(Color::WHITE) - rookMobility(Color::BLACK);
        features.rookBehindPassedPawn = rookBehindPassedPawn(Color::WHITE) - rookBehindPassedPawn(Color::BLACK);
        features.rookOpenFile = rookOpenFile(Color::WHITE) - rookOpenFile(Color::BLACK);
        features.rookSemiOpenFile = rookSemiOpenFile(Color::WHITE) - rookSemiOpenFile(Color::BLACK);
        features.rookAtckWeakPawnOpenColumn = rookAtckWeakPawnOpenColumn(Color::WHITE) - rookAtckWeakPawnOpenColumn(Color::BLACK);
        features.kingFriendlyPawn = kingFriendlyPawn(Color::WHITE) - kingFriendlyPawn(Color::BLACK);
        features.kingNoEnemyPawnNear = kingNoEnemyPawnNear(Color::WHITE) - kingNoEnemyPawnNear(Color::BLACK);
        features.kingPressureScore = kingPressureScore(Color::WHITE) - kingPressureScore(Color::BLACK);

    }


}; // end of FeatureExtractor class





