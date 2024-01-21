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

vector<Square> detectPassedPawns(Color col) {
    Bitboard pawns = board.pieces(PieceType::PAWN, col);
    Bitboard enemyPawns = board.pieces(PieceType::PAWN, opposite_color(col));

    vector<Square> passedPawns;
    // Check each pawn for passed pawn status
    while (pawns) {
        Square pawnSquare = chess::builtin::poplsb(pawns); // Get the least significant bit as Square
        Bitboard blockingSquares = blocking_squares(pawnSquare, col, enemyPawns);

        // Check if no enemy pawns are on the blocking squares
        if ((blockingSquares & enemyPawns) == 0) {
            std::cout << "Passed pawn at square " << squareToString[pawnSquare] << " for " << (col == Color::WHITE ? "White" : "Black") << std::endl;
            passedPawns.push_back(pawnSquare);
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


vector<Square> detectIsolatedPawns(Color color) {
    Bitboard pawns = board.pieces(PieceType::PAWN, color);

    vector<Square> isolatedPawns;

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


vector<Square> detectWeakPawns(Color color) {
    vector<Square> weakPawnList;
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
        weakPawnList.push_back(pawnSquare);

    }
    return weakPawnList;
}

// a weak square is a square that cannot be defended by a pawn and is the key three ranks in the center and on the enemy side of the board
//TODO: refine! 
Bitboard detectWeakSquares(Color color) {
    Bitboard pawns = board.pieces(PieceType::PAWN, color);
    Bitboard enemyPawns = board.pieces(PieceType::PAWN, opposite_color(color));

    Bitboard stops = (color == Color::WHITE) ? pawns << 8 : pawns >> 8;
    Bitboard attackSpans = (color == Color::WHITE) ? wAttackFrontSpans(pawns) : bAttackFrontSpans(pawns);
    Bitboard enemyAttacks = (color == Color::WHITE) ? bPawnAttacks(enemyPawns) : wPawnAttacks(enemyPawns);

    Bitboard weakSquares = (stops & enemyAttacks & ~attackSpans);
    weakSquares = (color == Color::WHITE) ? weakSquares >> 8 : weakSquares << 8;

    // Exclude edge columns (a and h)
    Bitboard edgeColumns = file_of(Square::SQ_A1) | file_of(Square::SQ_H1);
    weakSquares &= ~edgeColumns;

    return weakSquares;
}

//returns the number of passed pawns that a king could catch up to in time
int ruleOfTheSquare(Color color){
    vector<Square>passedPawns = detectPassedPawns(opposite_color(color));
    Square kingSquare = findPieceSquares(PieceType::KING, color)[0];
    
    int count = 0;
    //if the king is within the "square" of the pass pawn, then the king is in time to catch up to the pawn
    //if the king is outside the "square" of the pass pawn, then the king is too far away to catch up to the pawn
    if (passedPawns.size() == 0){
       return count;
    }
    for (int i = 0; i < passedPawns.size(); i++){
        Square passedPawnSquare = passedPawns[0];
        int passedPawnRank = rank_of(passedPawnSquare);
        int passedPawnFile = file_of(passedPawnSquare);
        int distanceToPromotion = 8 - passedPawnRank;
        if (color == Color::BLACK){
            distanceToPromotion = passedPawnRank;
        }

        

        int kingRank = rank_of(kingSquare);
        int kingFile = file_of(kingSquare);

        int distanceToStopPromotion = max(abs(kingRank - passedPawnRank), abs(kingFile - passedPawnFile));

        if (distanceToStopPromotion <= distanceToPromotion){
            count ++;
        }
    }
    return count;
}

//returns the number of knights on weak squares. This is not quite correct, because an outpost needs to be defended by a pawn, but it is a good approximation
int knightOutposts(Color color){
    vector<Square>knights = findPieceSquares(PieceType::KNIGHT, color);
    Bitboard wSquares = detectWeakSquares(opposite_color(color));
    int count = 0;
    for (int i = 0; i < knights.size(); i++){
        Square knightSquare = knights[i];
        if (wSquares & square_to_bitmap(knightSquare)){
            count ++;
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

short rookAttackKingFile(Color color){
    vector<Square> rooks = findPieceSquares(PieceType::ROOK, color);
    Square kingSquare = findPieceSquares(PieceType::KING, opposite_color(color))[0];
    int kingFile = file_of(kingSquare);
    short count = 0;
    for (int i = 0; i < rooks.size(); i++){
        Square rookSquare = rooks[i];
        int rookFile = file_of(rookSquare);
        if (rookFile == kingFile){
            count ++;
        }
    }
    return count;
}

short rookAttackKingAdjFile(Color color){
    vector<Square> rooks = findPieceSquares(PieceType::ROOK, color);
    Square kingSquare = findPieceSquares(PieceType::KING, opposite_color(color))[0];
    int kingFile = file_of(kingSquare);
    short count = 0;
    for (int i = 0; i < rooks.size(); i++){
        Square rookSquare = rooks[i];
        int rookFile = file_of(rookSquare);
        if (abs(rookFile - kingFile) == 1){
            count ++;
        }
    }
    return count;
}

short rookSeventhRank(Color color){
    vector<Square> rooks = findPieceSquares(PieceType::ROOK, color);
    short count = 0;
    for (int i = 0; i < rooks.size(); i++){
        Square rookSquare = rooks[i];
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
    vector<Square> rooks = findPieceSquares(PieceType::ROOK, color);
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

short rookBehindPassedPawn(Color color){
    vector<Square> rooks = findPieceSquares(PieceType::ROOK, color);
    vector<Square> passedPawns = detectPassedPawns(opposite_color(color));
    short count = 0;

    for (int i = 0; i < rooks.size(); i++){
        Square rookSquare = rooks[i];
        int rookRank = rank_of(rookSquare);
        int rookFile = file_of(rookSquare);

        for (int j = 0; j < passedPawns.size(); j++){
            Square passedPawnSquare = passedPawns[j];
            int passedPawnRank = rank_of(passedPawnSquare);
            int passedPawnFile = file_of(passedPawnSquare);

            if (rookFile == passedPawnFile){ // Check if rook and pawn are on the same file
                if (color == Color::WHITE){
                    if (rookRank < passedPawnRank){
                        count++;
                    }
                } else { // Color::BLACK
                    if (rookRank > passedPawnRank){
                        count++;
                    }
                }
            }
        }
    }
    return count;
}

short rookOpenFile(Color color){
    vector<Square> rooks = findPieceSquares(PieceType::ROOK, color);
    short count = 0;
    for (int i = 0; i < rooks.size(); i++){
        Square rookSquare = rooks[i];
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
    vector<Square> rooks = findPieceSquares(PieceType::ROOK, color);
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

//for all intents and purposes a weak pawn will just be a backwards pawn. 
short rookAtckWeakPawnOpenColumn(Color color){
    vector<Square> rooks = findPieceSquares(PieceType::ROOK, color);
    short count = 0;
    for (int i = 0; i < rooks.size(); i++){
        Square rookSquare = rooks[i];
        int rookFile = file_of(rookSquare);
        Bitboard fileMask = fileBitboard(rookFile);
        vector<Square> weakPawns = detectWeakPawns(opposite_color(color));
        for(int j = 0; j < weakPawns.size(); j++){
            Square weakPawnSquare = weakPawns[j];
            if (fileMask & square_to_bitmap(weakPawnSquare)){
                count ++;
            }
        }
    }
    return count;
}
// How many and how close are friendly pawns (closeness * numberofpawns / numberofpawns)
//only relevant to pawns on the same or adjacent files, the lower the score the better
// need to add a heav punishment if there is not three pawns, two or even one
short kingFriendlyPawn(Color color){
    vector<Square> friendlyPawns = findPieceSquares(PieceType::PAWN, color);
    Square kingSquare = findPieceSquares(PieceType::KING, color)[0];
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
    vector<Square> enemyPawns = findPieceSquares(PieceType::PAWN, opposite_color(color));
    Square kingSquare = findPieceSquares(PieceType::KING, color)[0];
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
    Square kingSquare = findPieceSquares(PieceType::KING, color)[0];
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

// a function that returns a score for how late into the game we are, and how many pieces are left
// again not optimal, because we've already calculated all these values
float endgameScore(){
    float score = 0;
    int numPieces = 0;
    vector<Square> whitePieces = findPieceSquares(PieceType::PAWN, Color::WHITE);
    vector<Square> blackPieces = findPieceSquares(PieceType::PAWN, Color::BLACK);
    numPieces += whitePieces.size();
    numPieces += blackPieces.size();
    whitePieces = findPieceSquares(PieceType::KNIGHT, Color::WHITE);
    blackPieces = findPieceSquares(PieceType::KNIGHT, Color::BLACK);
    numPieces += whitePieces.size() * 3;
    numPieces += blackPieces.size() * 3;
    whitePieces = findPieceSquares(PieceType::BISHOP, Color::WHITE);
    blackPieces = findPieceSquares(PieceType::BISHOP, Color::BLACK);
    numPieces += whitePieces.size() * 3;
    numPieces += blackPieces.size() * 3;
    whitePieces = findPieceSquares(PieceType::ROOK, Color::WHITE);
    blackPieces = findPieceSquares(PieceType::ROOK, Color::BLACK);
    numPieces += whitePieces.size() * 5;
    numPieces += blackPieces.size() * 5;
    whitePieces = findPieceSquares(PieceType::QUEEN, Color::WHITE);
    blackPieces = findPieceSquares(PieceType::QUEEN, Color::BLACK);
    numPieces += whitePieces.size() * 9;
    numPieces += blackPieces.size() * 9;
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
        
        features.weakPawns = detectWeakPawns(Color::WHITE).size() - detectWeakPawns(Color::BLACK).size();
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
        
        // endgamescore is how late into the game we are, and how many pieces are left
        features.endgameScore = endgameScore();

    }


}; // end of FeatureExtractor class





