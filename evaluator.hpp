//takes in a list of features from a position, and returns an evaluation score

// new architecture:
// one giant loop to get mobility scores, material, piece square tables, and other simple features
// then we send off the calculated bitboards to the feature extractor to get the more complicated features

//TODO:
// better more comprehensive king safety evaluation (maybe multiple things?) (attacking and defending scores?)
// knight mobility
// pawn islands
// queen mobility
// central pawns

#include <vector>
#include <string>
#include "chess.hpp"
#include "features.hpp"
#include "feature_extractor.hpp"
#include "baselines.hpp" 
#pragma once

using namespace chess;
using namespace std;
using namespace builtin;

#define FLIP(sq) (63 - sq) // stolen function to flip a square for BLACK/WHITE

// these are my human guesstimates or taken from the classic PeSTO engine

class Evaluator {



    public:

        Evaluator(Board& board, TunableEval featureWeights = baseEval) : board(board), featureWeights(featureWeights) {
            gamePhase = 0;
            fillKingSafetyTable(featureWeights.maxKingSafetyScore, static_cast<double>(featureWeights.steepnessKingSafetyScore / 100), featureWeights.middlePointKingSafetyScore);

        }

        void setFeatureWeights(TunableEval featureWeights){
            this->featureWeights = featureWeights;
            fillKingSafetyTable(featureWeights.maxKingSafetyScore, static_cast<double>(featureWeights.steepnessKingSafetyScore / 100), featureWeights.middlePointKingSafetyScore);
        }

        void fillKingSafetyTable(double L, double k, double x0) {
            for (int i = 0; i < 62; i++) {
                kingSafetyTable[i] = adjustedLogisticFunctionInt(i, L, k, x0);
            }
        }

        // Function to calculate the adjusted logistic function value for a given x and round it to the nearest integer
        int adjustedLogisticFunctionInt(double x, double L, double k, double x0) {
            // Calculate the logistic function and round to the nearest integer
            int y = std::round(L / (1 + exp(-k * (x - x0))));

            return y;
        }

        
        int getGamePhase(){
            return gamePhase;
        }

        //rethink how Im breaking up feature extraction and evaluation
        // might just be better to grab the pieces directly from the board here
        // and keep feature extraction to just the more complicated functions
        // we should never call this on a position that is a game over state

        //heavily influenced by the Raphael engine's implementation
        float evaluate(int depth, bool lazy = false){

        //bitboards we need

        Bitboard pieces = board.occ();
        Bitboard wPawns = 0;
        Bitboard bPawns = 0;
        Bitboard wKnights = 0;
        Bitboard bKnights = 0;
        Bitboard wBishops = 0;
        Bitboard bBishops = 0;
        Bitboard wRooks = 0;
        Bitboard bRooks = 0;
        Bitboard wQueens = 0;
        Bitboard bQueens = 0;
        Bitboard wKings = 0; // these are bitboards even though there can only be one
        Bitboard bKings = 0;

        //useful bitboards for pawn structure and the like
        Bitboard wPawnAttacks = 0;
        Bitboard bPawnAttacks = 0;

        Bitboard wKnightAttacks = 0;
        Bitboard bKnightAttacks = 0;

        Bitboard wBishopAttacks = 0;
        Bitboard bBishopAttacks = 0;

        Bitboard wRookAttacks = 0;
        Bitboard bRookAttacks = 0;

        Bitboard wQueenAttacks = 0;
        Bitboard bQueenAttacks = 0;

        
        // variables we need throughout the function
        float score = 0;
        int taperedEndgameScore = 0; 
        int mgscore = 0;
        int egscore = 0;


        // main eval loop
        for (int i = 0; i < 64; i++){
            Square sq = int_to_square(i);
            int sqi = i; // for the piece tables
            Piece piece = board.at(sq);

            // switch to handle each type of piece
            switch(piece){
                case Piece::NONE:
                    break;
                case Piece::WHITEPAWN:
                    wPawns |= square_to_bitmap(sq);
                    wPawnAttacks |= attacks::pawn(Color::WHITE, sq);
                    taperedEndgameScore += 1;
                    mgscore += featureWeights.pawn.middleGame;
                    egscore += featureWeights.pawn.endGame;
                    break;
                case Piece::BLACKPAWN:
                    bPawns |= square_to_bitmap(sq);
                    bPawnAttacks |= attacks::pawn(Color::BLACK, sq);
                    taperedEndgameScore += 1;
                    mgscore -= featureWeights.pawn.middleGame;
                    egscore -= featureWeights.pawn.endGame;
                    break;
                case Piece::WHITEKNIGHT:
                    wKnights |= square_to_bitmap(sq);
                    taperedEndgameScore += 3;
                    wKnightAttacks |= attacks::knight(sq);
                    mgscore += featureWeights.knight.middleGame;
                    egscore += featureWeights.knight.endGame;
                    break;
                case Piece::BLACKKNIGHT:
                    bKnights |= square_to_bitmap(sq);
                    taperedEndgameScore += 3;
                    bKnightAttacks |= attacks::knight(sq);
                    mgscore -= featureWeights.knight.middleGame;
                    egscore -= featureWeights.knight.endGame;
                    break;
                case Piece::WHITEBISHOP:
                    wBishops |= square_to_bitmap(sq);
                    taperedEndgameScore += 3;
                    wBishopAttacks |= attacks::bishop(sq, pieces);
                    mgscore += featureWeights.bishop.middleGame;
                    egscore += featureWeights.bishop.endGame;
                    break;
                case Piece::BLACKBISHOP:
                    bBishops |= square_to_bitmap(sq);
                    taperedEndgameScore += 3;
                    bBishopAttacks |= attacks::bishop(sq, pieces);
                    mgscore -= featureWeights.bishop.middleGame;
                    egscore -= featureWeights.bishop.endGame;
                    break;
                case Piece::WHITEROOK:
                    wRooks |= square_to_bitmap(sq);
                    taperedEndgameScore += 5;
                    wRookAttacks |= attacks::rook(sq, pieces);
                    mgscore += featureWeights.rook.middleGame;
                    egscore += featureWeights.rook.endGame;
                    break;
                case Piece::BLACKROOK:
                    bRooks |= square_to_bitmap(sq);
                    taperedEndgameScore += 5;
                    bRookAttacks |= attacks::rook(sq, pieces);
                    mgscore -= featureWeights.rook.middleGame;
                    egscore -= featureWeights.rook.endGame;
                    break;
                case Piece::WHITEQUEEN:
                    wQueens |= square_to_bitmap(sq);
                    taperedEndgameScore += 9;
                    wQueenAttacks |= attacks::queen(sq, pieces);
                    mgscore += featureWeights.queen.middleGame;
                    egscore += featureWeights.queen.endGame;
                    break;
                case Piece::BLACKQUEEN:
                    bQueens |= square_to_bitmap(sq);
                    taperedEndgameScore += 9;
                    bQueenAttacks |= attacks::queen(sq, pieces);
                    mgscore -= featureWeights.queen.middleGame;
                    egscore -= featureWeights.queen.endGame;
                    break;
                case Piece::WHITEKING:
                    wKings |= square_to_bitmap(sq);
                    break;
                case Piece::BLACKKING:
                    bKings |= square_to_bitmap(sq);
                    break;
            }
        }

        // more precise attack bitboards
        Bitboard allBPieces = bPawns | bKnights | bBishops | bRooks | bQueens | bKings;
        Bitboard allWPieces = wPawns | wKnights | wBishops | wRooks | wQueens | wKings;
        Bitboard allBPiecesXQueens = allBPieces & ~bQueens;
        Bitboard allWPiecesXQueens = allWPieces & ~wQueens;
        Bitboard allWPiecesXQR = allWPieces & ~(wQueens | wRooks);
        Bitboard allBPiecesXQR = allBPieces & ~(bQueens | bRooks);
        
        // various x-rays and stops
        wKnightAttacks &= ~allWPieces & ~allBPiecesXQR;
        bKnightAttacks &= ~allBPieces & ~allWPiecesXQR;
        wBishopAttacks &= ~allWPieces & ~allBPiecesXQR;
        bBishopAttacks &= ~allBPieces & ~allWPiecesXQR;
        wRookAttacks &= ~allWPieces & ~allBPiecesXQueens;
        bRookAttacks &= ~allBPieces & ~allWPiecesXQueens;
        wQueenAttacks &= ~allWPieces;
        bQueenAttacks &= ~allBPieces;

        
        // Calculate the game phase dynamically based on the endgame score
        gamePhase = std::max(0.0f, std::min(1.0f, (taperedEndgameScore - 24) / 24.0f)); // Ensure the game phase is between 0 and 1
        
        float mgWeight = gamePhase;
        float egWeight = 1 - gamePhase;
        
        // Combine middle game and end game scores based on the current game phase
        score = mgscore * mgWeight + egscore * egWeight;

        // if lazy evaluation is enabled, return the score here
        // we can also be lazy in evaluating completely winning positions not in the endgame
        if(lazy){
            return score;
        }

        // extract the rest of the features from the board

        // passed pawns (tested and working)
        Bitboard whitePassedPawns = detectPassedPawns(Color::WHITE, wPawns, bPawns);
        Bitboard blackPassedPawns = detectPassedPawns(Color::BLACK, bPawns, wPawns);
        score += (builtin::popcount(whitePassedPawns) - builtin::popcount(blackPassedPawns)) * (featureWeights.passedPawn.middleGame * mgWeight + featureWeights.passedPawn.endGame * egWeight);

        // doubled pawns (tested and working)
        // adjusting the score negatively for doubled pawns
        score -= (detectDoubledPawns(Color::BLACK, wPawns, bPawns) - detectDoubledPawns(Color::WHITE, bPawns, wPawns)) * (featureWeights.doubledPawn.middleGame * mgWeight + featureWeights.doubledPawn.endGame * egWeight);

        Bitboard isolatedWhitePawns = detectIsolatedPawns(wPawns);
        Bitboard isolatedBlackPawns = detectIsolatedPawns(bPawns);
        // isolated pawns (tested and working)
        // adjusting the score negatively for isolated pawns
        score -= (builtin::popcount(isolatedWhitePawns) - builtin::popcount(isolatedBlackPawns)) * (featureWeights.isolatedPawn.middleGame * mgWeight + featureWeights.isolatedPawn.endGame * egWeight);


        // weak pawns (finally working)
        // adjusting the score negatively for weak pawns
        Bitboard weakWhitePawns = wBackward(wPawns, bPawns);
        Bitboard weakBlackPawns = bBackward(bPawns, wPawns);
        
        // exclude passers and isolated pawns from weak pawns
        weakWhitePawns &= ~whitePassedPawns & ~isolatedWhitePawns;
        weakBlackPawns &= ~blackPassedPawns & ~isolatedBlackPawns;

        score -= (builtin::popcount(weakBlackPawns) - builtin::popcount(weakWhitePawns)) * (featureWeights.weakPawn.middleGame * mgWeight + featureWeights.weakPawn.endGame * egWeight);

        // central pawns (not tested)
        score += (builtin::popcount(detectCentralPawns(wPawns)) - builtin::popcount(detectCentralPawns(bPawns))) * (featureWeights.centralPawn.middleGame * mgWeight + featureWeights.centralPawn.endGame * egWeight);

        // weak squares (working finally)
        // adjusting the score negatively for weak squares
        Bitboard weakWhiteSquares = detectWeakSquares(Color::WHITE, wPawns);
        Bitboard weakBlackSquares = detectWeakSquares(Color::BLACK, bPawns);
        score -= (builtin::popcount(weakBlackSquares) - builtin::popcount(weakWhiteSquares)) * (featureWeights.weakSquare.middleGame * mgWeight + featureWeights.weakSquare.endGame * egWeight);

        // rule of the square (tested and working)
        score += (ruleOfTheSquare(Color::WHITE, blackPassedPawns, wKings) - ruleOfTheSquare(Color::BLACK, whitePassedPawns, bKings)) * (featureWeights.passedPawnEnemyKingSquare.middleGame * mgWeight + featureWeights.passedPawnEnemyKingSquare.endGame * egWeight);
        
        // knight outposts (tested like 90% sure it works)
        score += (builtin::popcount(knightOutposts(weakBlackSquares, wKnights, wPawnAttacks)) - builtin::popcount(knightOutposts(weakWhiteSquares, bKnights, bPawnAttacks))) * (featureWeights.knightOutposts.middleGame * mgWeight + featureWeights.knightOutposts.endGame * egWeight);

        // knight mobility (not tested)
        score += (knightMobility(wKnightAttacks) - knightMobility(bKnightAttacks)) * (featureWeights.knightMobility.middleGame * mgWeight + featureWeights.knightMobility.endGame * egWeight);

        // bishop mobility (tested)
        score += (bishopMobility(wBishopAttacks) - bishopMobility(bBishopAttacks)) * (featureWeights.bishopMobility.middleGame * mgWeight + featureWeights.bishopMobility.endGame * egWeight);
        

        // bishop pair (tested and working)
        score += (bishopPair(wBishops) - bishopPair(bBishops)) * (featureWeights.bishopPair.middleGame * mgWeight + featureWeights.bishopPair.endGame * egWeight);
        

        // rook attack king file (tested and fixed)
        score += (rookAttackKingFile(Color::WHITE, wRooks, bKings) - rookAttackKingFile(Color::BLACK, bRooks, wKings)) * (featureWeights.rookAttackKingFile.middleGame * mgWeight + featureWeights.rookAttackKingFile.endGame * egWeight);

        // rook attack king adjacent file (tested and fixed)
        score += (rookAttackKingAdjFile(Color::WHITE, wRooks, bKings) - rookAttackKingAdjFile(Color::BLACK, bRooks, wKings)) * (featureWeights.rookAttackKingAdjFile.middleGame * mgWeight + featureWeights.rookAttackKingAdjFile.endGame * egWeight);


        // rook on 7th rank (tested and working)
        score += (rookSeventhRank(Color::WHITE, wRooks) - rookSeventhRank(Color::BLACK, bRooks)) * (featureWeights.rook7thRank.middleGame * mgWeight + featureWeights.rook7thRank.endGame * egWeight);
        

        // rook connected (tested and working)
        score += (rookConnected(Color::WHITE, wRooks, pieces) - rookConnected(Color::BLACK, bRooks, pieces)) * (featureWeights.rookConnected.middleGame * mgWeight + featureWeights.rookConnected.endGame * egWeight);
        

        // rook mobility (tested and working)
        score += (rookMobility(wRookAttacks) - rookMobility(bRookAttacks)) * (featureWeights.rookMobility.middleGame * mgWeight + featureWeights.rookMobility.endGame * egWeight);


        // rook behind passed pawn (tested and working) 
        score += (rookBehindPassedPawn(Color::WHITE, wRooks, whitePassedPawns) - rookBehindPassedPawn(Color::BLACK, bRooks, blackPassedPawns)) * (featureWeights.rookBehindPassedPawn.middleGame * mgWeight + featureWeights.rookBehindPassedPawn.endGame * egWeight);
        

        // rook on open file
        Bitboard allPawns = wPawns | bPawns;
        score += (rookOpenFile(Color::WHITE, wRooks, allPawns) - rookOpenFile(Color::BLACK, bRooks, allPawns)) * (featureWeights.rookOpenFile.middleGame * mgWeight + featureWeights.rookOpenFile.endGame * egWeight);

        // rook on semi-open file
        score += (rookSemiOpenFile(Color::WHITE, wRooks, wPawns, bPawns) - rookSemiOpenFile(Color::BLACK, bRooks, bPawns, wPawns)) * (featureWeights.rookSemiOpenFile.middleGame * mgWeight + featureWeights.rookSemiOpenFile.endGame * egWeight);

        // rook attack weak pawn on open column
        score += (rookAtckWeakPawnOpenColumn(Color::WHITE, wRooks, weakBlackPawns) - rookAtckWeakPawnOpenColumn(Color::BLACK, bRooks, weakWhitePawns)) * (featureWeights.rookAtckWeakPawnOpenColumn.middleGame * mgWeight + featureWeights.rookAtckWeakPawnOpenColumn.endGame * egWeight);

        // queen mobility (not tested)
        score += (queenMobility(wQueenAttacks) - queenMobility(bQueenAttacks)) * (featureWeights.queenMobility.middleGame * mgWeight + featureWeights.queenMobility.endGame * egWeight);

        // king friendly pawn
        score += (kingFriendlyPawn(wPawns, wKings) - kingFriendlyPawn(bPawns, bKings)) * (featureWeights.kingFriendlyPawn.middleGame * mgWeight + featureWeights.kingFriendlyPawn.endGame * egWeight);

        // king no enemy pawn near
        score += (kingNoEnemyPawnNear(bPawns, wKings) - kingNoEnemyPawnNear(wPawns, bKings)) * (featureWeights.kingNoEnemyPawnNear.middleGame * mgWeight + featureWeights.kingNoEnemyPawnNear.endGame * egWeight);

        // revised king pressure scores  (yet to be tested)
        // score += kingPressureScore(wKings, bKnightAttacks, bBishopAttacks, bRookAttacks, bQueenAttacks, board, Color::WHITE, featureWeights.kingSafetyTable);
        // score -= kingPressureScore(bKings, wKnightAttacks, wBishopAttacks, wRookAttacks, wQueenAttacks, board, Color::BLACK, featureWeights.kingSafetyTable);
        return score;
        }

    // used for testing only, comma delimited string of features, 
    // format: 
    // featurename[WVALUE:BVALUE]:totalchangeinevaluation, 

    private:
        TunableEval featureWeights;
        float gamePhase;
        Board& board;
        int kingSafetyTable[62];

        static Color color(Piece piece) {
            return static_cast<Color>(static_cast<int>(piece) / 6);
        }

        

        



};