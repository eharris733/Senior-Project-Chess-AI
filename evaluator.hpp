//takes in a list of features from a position, and returns an evaluation score

// new architecture:
// one giant loop to get mobility scores, material, piece square tables, and other simple features
// then we send off the calculated bitboards to the feature extractor to get the more complicated features

#include <vector>
#include <string>
#include "chess.hpp"
#include "features.hpp"
#include "feature_extractor.hpp"
#include "tunableValues.hpp"   
#pragma once

using namespace chess;
using namespace std;
using namespace builtin;

#define FLIP(sq) (63 - sq) // stolen function to flip a square for BLACK/WHITE

// these are my human guesstimates or taken from the classic PeSTO engine
TunableValues baseline = {
    //piece tables are taken from the classic PeSTO engine
    //https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function
    constants::STARTPOS, // The FEN string
    {
      0,   0,   0,   0,   0,   0,  0,   0,
     98, 134,  61,  95,  68, 126, 34, -11,
     -6,   7,  26,  31,  65,  56, 25, -20,
    -14,  13,   6,  21,  23,  12, 17, -23,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -35,  -1, -20, -23, -15,  24, 38, -22,
      0,   0,   0,   0,   0,   0,  0,   0,
    },
    {
      0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
     94, 100,  85,  67,  56,  53,  82,  84,
     32,  24,  13,   5,  -2,   4,  17,  17,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   8,   8,  10,  13,   0,   2,  -7,
      0,   0,   0,   0,   0,   0,   0,   0,
    },

    {
    -167, -89, -34, -49,  61, -97, -15, -107,
     -73, -41,  72,  36,  23,  62,   7,  -17,
     -47,  60,  37,  65,  84, 129,  73,   44,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -13,   4,  16,  13,  28,  19,  21,   -8,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
    -105, -21, -58, -33, -17, -28, -19,  -23,
    },
    {
    -58, -38, -13, -28, -31, -27, -63, -99,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -29, -51, -23, -15, -22, -18, -50, -64,
    },
    {
    -29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
     -4,   5,  19,  50,  37,  37,   7,  -2,
     -6,  13,  13,  26,  34,  12,  10,   4,
      0,  15,  15,  15,  14,  27,  18,  10,
      4,  15,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21,
    },
    {
    -14, -21, -11,  -8, -7,  -9, -17, -24,
     -8,  -4,   7, -12, -3, -13,  -4, -14,
      2,  -8,   0,  -1, -2,   6,   0,   4,
     -3,   9,  12,   9, 14,  10,   3,   2,
     -6,   3,  13,  19,  7,  10,  -3,  -9,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
    },
    {
     32,  42,  32,  51, 63,  9,  31,  43,
     27,  32,  58,  62, 80, 67,  26,  44,
     -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26,
    },
    {
    13, 10, 18, 15, 12,  12,   8,   5,
    11, 13, 13, 11, -3,   3,   8,   3,
     7,  7,  7,  5,  4,  -3,  -5,  -3,
     4,  3, 13,  1,  2,   1,  -1,   2,
     3,  5,  8,  4, -5,  -6,  -8, -11,
    -4,  0, -5, -1, -7, -12,  -8, -16,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -9,  2,  3, -1, -5, -13,   4, -20,
    },
    {
    -28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
     -1, -18,  -9,  10, -15, -25, -31, -50,
    },
    {
     -9,  22,  22,  27,  27,  19,  10,  20,
    -17,  20,  32,  41,  58,  25,  30,   0,
    -20,   6,   9,  49,  47,  35,  19,   9,
      3,  22,  24,  45,  57,  40,  57,  36,
    -18,  28,  19,  47,  31,  34,  39,  23,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -43,  -5, -32, -20, -41,
    },
    {
    -65,  23,  16, -15, -56, -34,   2,  13,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
     -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  36,  12, -54,   8, -28,  24,  14,
    },
    {
    -74, -35, -18, -18, -11,  15,   4, -17,
    -12,  17,  14,  17,  17,  38,  23,  11,
     10,  17,  23,  15,  20,  45,  44,  13,
     -8,  22,  24,  27,  26,  33,  26,   3,
    -18,  -4,  21,  24,  27,  23,   9, -11,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
    },

    //piece values
    GamePhaseValue(1000, 1000), // Pawn
    GamePhaseValue(3050, 3000), // Knight
    GamePhaseValue(3150, 3200), // Bishop
    GamePhaseValue(4800, 5200), // Rook
    GamePhaseValue(9100, 9100), // Queen

    //values are completely guessed based on whims
    GamePhaseValue(40, 80),   // Passed pawn
    GamePhaseValue(-10, -20), // Doubled pawn (not good)
    GamePhaseValue(-20, -20), // Isolated pawn (not good)
    GamePhaseValue(-40, -40), // Weak pawn
    GamePhaseValue(-5, -2), // Weak square
    GamePhaseValue(0, 50), // Rule of the square
    GamePhaseValue(40, 35), // Knight is on a weak square
    GamePhaseValue(3, 1), // How many squares a bishop can move to
    GamePhaseValue(15, 20), // Possessing both colored bishops
    GamePhaseValue(15, 5), // Rook is on same file as enemy king
    GamePhaseValue(10, 5), // Rook is on adjacent file to enemy king
    GamePhaseValue(25, 35), // Rook is on second to last rank
    GamePhaseValue(20, 10), // Rooks can guard each other
    GamePhaseValue(5, 1), // How many squares a rook has
    GamePhaseValue(10, 35), // Rook is behind a passed pawn
    GamePhaseValue(15, 5), // Rook has no pawns in its file
    GamePhaseValue(20, 10), // Rook has no friendly pawns in its file
    GamePhaseValue(35, 30), // Rook can directly attack a weak enemy pawn
    GamePhaseValue(4, 0), // How many and how close are friendly pawns (closeness * numberofpawns / numberofpawns)
    GamePhaseValue(5, 0), // How far king is from closest enemy pawn
    GamePhaseValue(10, 10), // Multiplier for king pressure

    //tuneable search parameters
    {100, 350}, //aspiration window progression
    20, //aspiration window initial delta
    5, // use aspiration window depth
    true, // use lazy eval for null move reductions
    {300, 900, 1300}, // futility pruning margin
    true, // use futility pruning lazy eval
    300, // delta pruning in QS search
    200, // promotion score
    100, // killer move score
    10, // base move score

};

class Evaluator {



    public:

        Evaluator(Board& board, TunableValues featureWeights = baseline) : board(board), featureWeights(featureWeights) {
            gamePhase = 0;
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

        
        // variables we need
        float score = 0;
        int taperedEndgameScore = 0; // Use a direct integer instead of a pointer for simplicity
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
                    mgscore += featureWeights.pawnsMG[sqi] + featureWeights.pawn.middleGame;
                    egscore += featureWeights.pawnsEG[sqi] + featureWeights.pawn.endGame;
                    break;
                case Piece::BLACKPAWN:
                    bPawns |= square_to_bitmap(sq);
                    bPawnAttacks |= attacks::pawn(Color::BLACK, sq);
                    taperedEndgameScore += 1;
                    mgscore -= featureWeights.pawnsMG[FLIP(sqi)] + featureWeights.pawn.middleGame;
                    egscore -= featureWeights.pawnsEG[FLIP(sqi)] + featureWeights.pawn.endGame;
                    break;
                case Piece::WHITEKNIGHT:
                    wKnights |= square_to_bitmap(sq);
                    taperedEndgameScore += 3;
                    wKnightAttacks |= attacks::knight(sq);
                    mgscore += featureWeights.knightsMG[sqi] + featureWeights.knight.middleGame;
                    egscore += featureWeights.knightsEG[sqi] + featureWeights.knight.endGame;
                    break;
                case Piece::BLACKKNIGHT:
                    bKnights |= square_to_bitmap(sq);
                    taperedEndgameScore += 3;
                    bKnightAttacks |= attacks::knight(sq);
                    mgscore -= featureWeights.knightsMG[FLIP(sqi)] + featureWeights.knight.middleGame;
                    egscore -= featureWeights.knightsEG[FLIP(sqi)] + featureWeights.knight.endGame;
                    break;
                case Piece::WHITEBISHOP:
                    wBishops |= square_to_bitmap(sq);
                    taperedEndgameScore += 3;
                    wBishopAttacks |= attacks::bishop(sq, pieces);
                    mgscore += featureWeights.bishopsMG[sqi] + featureWeights.bishop.middleGame;
                    egscore += featureWeights.bishopsEG[sqi] + featureWeights.bishop.endGame;
                    break;
                case Piece::BLACKBISHOP:
                    bBishops |= square_to_bitmap(sq);
                    taperedEndgameScore += 3;
                    bBishopAttacks |= attacks::bishop(sq, pieces);
                    mgscore -= featureWeights.bishopsMG[FLIP(sqi)] + featureWeights.bishop.middleGame;
                    egscore -= featureWeights.bishopsEG[FLIP(sqi)] + featureWeights.bishop.endGame;
                    break;
                case Piece::WHITEROOK:
                    wRooks |= square_to_bitmap(sq);
                    taperedEndgameScore += 5;
                    wRookAttacks |= attacks::rook(sq, pieces);
                    mgscore += featureWeights.rooksMG[sqi] + featureWeights.rook.middleGame;
                    egscore += featureWeights.rooksEG[sqi] + featureWeights.rook.endGame;
                    break;
                case Piece::BLACKROOK:
                    bRooks |= square_to_bitmap(sq);
                    taperedEndgameScore += 5;
                    bRookAttacks |= attacks::rook(sq, pieces);
                    mgscore -= featureWeights.rooksMG[FLIP(sqi)] + featureWeights.rook.middleGame;
                    egscore -= featureWeights.rooksEG[FLIP(sqi)] + featureWeights.rook.endGame;
                    break;
                case Piece::WHITEQUEEN:
                    wQueens |= square_to_bitmap(sq);
                    taperedEndgameScore += 9;
                    wQueenAttacks |= attacks::queen(sq, pieces);
                    mgscore += featureWeights.queensMG[sqi] + featureWeights.queen.middleGame;
                    egscore += featureWeights.queensEG[sqi] + featureWeights.queen.endGame;
                    break;
                case Piece::BLACKQUEEN:
                    bQueens |= square_to_bitmap(sq);
                    taperedEndgameScore += 9;
                    bQueenAttacks |= attacks::queen(sq, pieces);
                    mgscore -= featureWeights.queensMG[FLIP(sqi)] + featureWeights.queen.middleGame;
                    egscore -= featureWeights.queensEG[FLIP(sqi)] + featureWeights.queen.endGame;
                    break;
                case Piece::WHITEKING:
                    wKings |= square_to_bitmap(sq);
                    break;
                case Piece::BLACKKING:
                    bKings |= square_to_bitmap(sq);
                    break;
            }
        }

        // divide by ten to make the piece tables less powerful
        mgscore /= 10;
        egscore /= 10;
        
        // Calculate the game phase dynamically based on the endgame score
        gamePhase = std::max(0.0f, std::min(1.0f, (taperedEndgameScore - 24) / 24.0f)); // Ensure the game phase is between 0 and 1
        
        float mgWeight = gamePhase;
        float egWeight = 1 - gamePhase;
        
        // Combine middle game and end game scores based on the current game phase
        score = mgscore * mgWeight + egscore * egWeight;

        // if lazy evaluation is enabled, return the score here
        // we can also be lazy in evaluating completely winning positions not in the endgame
        if(lazy || ((abs(score) > 500 && gamePhase > .2))){
            return score;
        }

        // extract the rest of the features from the board

        // passed pawns (tested and working)
        Bitboard whitePassedPawns = detectPassedPawns(Color::WHITE, wPawns, bPawns);
        Bitboard blackPassedPawns = detectPassedPawns(Color::BLACK, bPawns, wPawns);
        score += (builtin::popcount(whitePassedPawns) - builtin::popcount(blackPassedPawns)) * (featureWeights.passedPawn.middleGame * mgWeight + featureWeights.passedPawn.endGame * egWeight);

        // doubled pawns (tested and working)
        score += (detectDoubledPawns(Color::WHITE, wPawns, bPawns) - detectDoubledPawns(Color::BLACK, bPawns, wPawns)) * (featureWeights.doubledPawn.middleGame * mgWeight + featureWeights.doubledPawn.endGame * egWeight);

        Bitboard isolatedWhitePawns = detectIsolatedPawns(wPawns);
        Bitboard isolatedBlackPawns = detectIsolatedPawns(bPawns);
        // isolated pawns (tested and working)
        score += (builtin::popcount(isolatedWhitePawns) - builtin::popcount(isolatedBlackPawns)) * (featureWeights.isolatedPawn.middleGame * mgWeight + featureWeights.isolatedPawn.endGame * egWeight);


        // weak pawns (finally working)
        Bitboard weakWhitePawns = wBackward(wPawns, bPawns);
        Bitboard weakBlackPawns = bBackward(bPawns, wPawns);
        
        // exclude passers and isolated pawns from weak pawns
        weakWhitePawns &= ~whitePassedPawns & ~isolatedWhitePawns;
        weakBlackPawns &= ~blackPassedPawns & ~isolatedBlackPawns;

        score += (popcount(weakWhitePawns) - popcount(weakBlackPawns)) * (featureWeights.weakPawn.middleGame * mgWeight + featureWeights.weakPawn.endGame * egWeight);

        // weak squares (working finally)
        Bitboard weakWhiteSquares = detectWeakSquares(Color::WHITE, wPawns);
        Bitboard weakBlackSquares = detectWeakSquares(Color::BLACK, bPawns);
        score += (popcount(weakWhiteSquares) - popcount(weakBlackSquares)) * (featureWeights.weakSquare.middleGame * mgWeight + featureWeights.weakSquare.endGame * egWeight);

        // rule of the square (tested and working)
        score += (ruleOfTheSquare(Color::WHITE, blackPassedPawns, wKings) - ruleOfTheSquare(Color::BLACK, whitePassedPawns, bKings)) * (featureWeights.passedPawnEnemyKingSquare.middleGame * mgWeight + featureWeights.passedPawnEnemyKingSquare.endGame * egWeight);
        
        // knight outposts (tested like 90% sure it works)
        score += (popcount(knightOutposts(weakBlackSquares, wKnights, wPawnAttacks)) - popcount(knightOutposts(weakWhiteSquares, bKnights, bPawnAttacks))) * (featureWeights.knightOutposts.middleGame * mgWeight + featureWeights.knightOutposts.endGame * egWeight);

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

        // king friendly pawn
        score += (kingFriendlyPawn(wPawns, wKings) - kingFriendlyPawn(bPawns, bKings)) * (featureWeights.kingFriendlyPawn.middleGame * mgWeight + featureWeights.kingFriendlyPawn.endGame * egWeight);

        // king no enemy pawn near
        score += (kingNoEnemyPawnNear(bPawns, wKings) - kingNoEnemyPawnNear(wPawns, bKings)) * (featureWeights.kingNoEnemyPawnNear.middleGame * mgWeight + featureWeights.kingNoEnemyPawnNear.endGame * egWeight);

        // king pressure score
        score += (kingPressureScore(wKings, bPawns | bKnights | bBishops | bRooks | bQueens, board) - kingPressureScore(bKings, wPawns | wKnights | wBishops | wRooks | wQueens, board)) * (featureWeights.kingPressureScore.middleGame * mgWeight + featureWeights.kingPressureScore.endGame * egWeight);

        return score;
        }

    private:
        TunableValues featureWeights;
        float gamePhase;
        Board& board;

        //helper function to get the color of a piece
        static Color color(Piece piece) {
            return static_cast<Color>(static_cast<int>(piece) / 6);
        }


};