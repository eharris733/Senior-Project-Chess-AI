//takes in a list of features from a position, and returns an evaluation score

#include <vector>
#include <string>
#include "chess.hpp"
#include "features.hpp"
#include "feature_extractor.hpp"

using namespace chess;
using namespace std;

// a struct to allow each feature to be weighted differently based on phase of the game
struct GamePhaseValue {
    int middleGame;
    int endGame;

    GamePhaseValue(int mg = 0, int eg = 0) : middleGame(mg), endGame(eg) {}
};


struct weightFeaturesByHand{
    //piece tables are taken from the classic PeSTO engine
    //https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function
    string fen; // The FEN string
    int pawnsMG[64] = {
        0,   0,   0,   0,   0,   0,  0,   0,
        98, 134,  61,  95,  68, 126, 34, -11,
        -6,   7,  26,  31,  65,  56, 25, -20,
        -14,  13,   6,  21,  23,  12, 17, -23,
        -27,  -2,  -5,  12,  17,   6, 10, -25,
        -26,  -4,  -4, -10,   3,   3, 33, -12,
        -35,  -1, -20, -23, -15,  24, 38, -22,
        0,   0,   0,   0,   0,   0,  0,   0,
    };
    int pawnsEG[64] = {
        0,   0,   0,   0,   0,   0,   0,   0,
        178, 173, 158, 134, 147, 132, 165, 187,
        94, 100,  85,  67,  56,  53,  82,  84,
        32,  24,  13,   5,  -2,   4,  17,  17,
        13,   9,  -3,  -7,  -7,  -8,   3,  -1,
        4,   7,  -6,   1,   0,  -5,  -1,  -8,
        13,   8,   8,  10,  13,   0,   2,  -7,
        0,   0,   0,   0,   0,   0,   0,   0,
    };
    int knightsMG[64] = {
        -167, -89, -34, -49,  61, -97, -15, -107,
        -73, -41,  72,  36,  23,  62,   7,  -17,
        -47,  60,  37,  65,  84, 129,  73,   44,
        -9,  17,  19,  53,  37,  69,  18,   22,
        -13,   4,  16,  13,  28,  19,  21,   -8,
        -23,  -9,  12,  10,  19,  17,  25,  -16,
        -29, -53, -12,  -3,  -1,  18, -14,  -19,
        -105, -21, -58, -33, -17, -28, -19,  -23,
    };
    int knightsEG[64] = {
        -58, -38, -13, -28, -31, -27, -63, -99,
        -25,  -8, -25,  -2,  -9, -25, -24, -52,
        -24, -20,  10,   9,  -1,  -9, -19, -41,
        -17,   3,  22,  22,  22,  11,   8, -18,
        -18,  -6,  16,  25,  16,  17,   4, -18,
        -23,  -3,  -1,  15,  10,  -3, -20, -22,
        -42, -20, -10,  -5,  -2, -20, -23, -44,
        -29, -51, -23, -15, -22, -18, -50, -64,
    };
    int bishopsMG[64] = {
        -29,   4, -82, -37, -25, -42,   7,  -8,
        -26,  16, -18, -13,  30,  59,  18, -47,
        -16,  37,  43,  40,  35,  50,  37,  -2,
        -4,   5,  19,  50,  37,  37,   7,  -2,
        -6,  13,  13,  26,  34,  12,  10,   4,
        0,  15,  15,  15,  14,  27,  18,  10,
        4,  15,  16,   0,   7,  21,  33,   1,
        -33,  -3, -14, -21, -13, -12, -39, -21,
    };
    int bishopsEG[64] = {
        -14, -21, -11,  -8, -7,  -9, -17, -24,
        -8,  -4,   7, -12, -3, -13,  -4, -14,
        2,  -8,   0,  -1, -2,   6,   0,   4,
        -3,   9,  12,   9, 14,  10,   3,   2,
        -6,   3,  13,  19,  7,  10,  -3,  -9,
        -12,  -3,   8,  10, 13,   3,  -7, -15,
        -14, -18,  -7,  -1,  4,  -9, -15, -27,
        -23,  -9, -23,  -5, -9, -16,  -5, -17,
    };
    int rooksMG[64] = {
        32,  42,  32,  51, 63,  9,  31,  43,
        27,  32,  58,  62, 80, 67,  26,  44,
        -5,  19,  26,  36, 17, 45,  61,  16,
        -24, -11,   7,  26, 24, 35,  -8, -20,
        -36, -26, -12,  -1,  9, -7,   6, -23,
        -45, -25, -16, -17,  3,  0,  -5, -33,
        -44, -16, -20,  -9, -1, 11,  -6, -71,
        -19, -13,   1,  17, 16,  7, -37, -26,
    };
    int rooksEG[64] = {
        13, 10, 18, 15, 12,  12,   8,   5,
        11, 13, 13, 11, -3,   3,   8,   3,
        7,  7,  7,  5,  4,  -3,  -5,  -3,
        4,  3, 13,  1,  2,   1,  -1,   2,
        3,  5,  8,  4, -5,  -6,  -8, -11,
        -4,  0, -5, -1, -7, -12,  -8, -16,
        -6, -6,  0,  2, -9,  -9, -11,  -3,
        -9,  2,  3, -1, -5, -13,   4, -20,
    };
    int queensMG[64] = {
        -28,   0,  29,  12,  59,  44,  43,  45,
        -24, -39,  -5,   1, -16,  57,  28,  54,
        -13, -17,   7,   8,  29,  56,  47,  57,
        -27, -27, -16, -16,  -1,  17,  -2,   1,
        -9, -26,  -9, -10,  -2,  -4,   3,  -3,
        -14,   2, -11,  -2,  -5,   2,  14,   5,
        -35,  -8,  11,   2,   8,  15,  -3,   1,
        -1, -18,  -9,  10, -15, -25, -31, -50,
    };
    int queensEG[64] = {
        -9,  22,  22,  27,  27,  19,  10,  20,
        -17,  20,  32,  41,  58,  25,  30,   0,
        -20,   6,   9,  49,  47,  35,  19,   9,
        3,  22,  24,  45,  57,  40,  57,  36,
        -18,  28,  19,  47,  31,  34,  39,  23,
        -16, -27,  15,   6,   9,  17,  10,   5,
        -22, -23, -30, -16, -16, -23, -36, -32,
        -33, -28, -22, -43,  -5, -32, -20, -41,
    };
    int kingsMG[64] = {
        -65,  23,  16, -15, -56, -34,   2,  13,
        29,  -1, -20,  -7,  -8,  -4, -38, -29,
        -9,  24,   2, -16, -20,   6,  22, -22,
        -17, -20, -12, -27, -30, -25, -14, -36,
        -49,  -1, -27, -39, -46, -44, -33, -51,
        -14, -14, -22, -46, -44, -30, -15, -27,
        1,   7,  -8, -64, -43, -16,   9,   8,
        -15,  36,  12, -54,   8, -28,  24,  14,
    };
    int kingsEG[64] = {
        -74, -35, -18, -18, -11,  15,   4, -17,
        -12,  17,  14,  17,  17,  38,  23,  11,
        10,  17,  23,  15,  20,  45,  44,  13,
        -8,  22,  24,  27,  26,  33,  26,   3,
        -18,  -4,  21,  24,  27,  23,   9, -11,
        -19,  -3,  11,  21,  23,  16,   7,  -9,
        -27, -11,   4,  13,  14,   4,  -5, -17,
        -53, -34, -21, -11, -28, -14, -24, -43,
    };

    //values are completely guessed based on whims
    GamePhaseValue passedPawn = GamePhaseValue(80, 160);   // Passed pawn
    GamePhaseValue doubledPawn = GamePhaseValue(20, 40); // Doubled pawn
    GamePhaseValue isolatedPawn = GamePhaseValue(20, 40); // Isolated pawn
    GamePhaseValue weakPawn = GamePhaseValue(40, 20); // Weak pawn
    GamePhaseValue weakSquare = GamePhaseValue(10, 0); // Weak square
    GamePhaseValue passedPawnEnemyKingSquare = GamePhaseValue(0, 100); // Rule of the square
    GamePhaseValue knightOutposts = GamePhaseValue(40, 10); // Knight is on a weak square
    GamePhaseValue bishopMobility = GamePhaseValue(40, 40); // How many squares a bishop can move to
    GamePhaseValue bishopPair = GamePhaseValue(40, 40); // Possessing both colored bishops
    GamePhaseValue rookAttackKingFile = GamePhaseValue(30, 10); // Rook is on same file as enemy king
    GamePhaseValue rookAttackKingAdjFile = GamePhaseValue(20, 10); // Rook is on adjacent file to enemy king
    GamePhaseValue rook7thRank = GamePhaseValue(50, 70); // Rook is on second to last rank
    GamePhaseValue rookConnected = GamePhaseValue(20, 20); // Rooks can guard each other
    GamePhaseValue rookMobility = GamePhaseValue(40, 10); // How many squares a rook has
    GamePhaseValue rookBehindPassedPawn = GamePhaseValue(20, 50); // Rook is behind a passed pawn
    GamePhaseValue rookOpenFile = GamePhaseValue(30, 10); // Rook has no pawns in its file
    GamePhaseValue rookSemiOpenFile = GamePhaseValue(20, 20); // Rook has no friendly pawns in its file
    GamePhaseValue rookAtckWeakPawnOpenColumn = GamePhaseValue(50, 60); // Rook can directly attack a weak enemy pawn
    GamePhaseValue kingFriendlyPawn = GamePhaseValue(60, 0); // How many and how close are friendly pawns (closeness * numberofpawns / numberofpawns)
    GamePhaseValue kingNoEnemyPawnNear = GamePhaseValue(40, 10); // How far king is from closest enemy pawn
    GamePhaseValue kingPressureScore = GamePhaseValue(30, 10); // Multiplier for king pressure
};

class Evaluator {
    private:
    Board board;
    weightFeaturesByHand weightFeaturesByHand;

    public:
    Evaluator(Board& b) : board(b) {}

    //rethink how Im breaking up feature extraction and evaluation
    // might just be better to grab the pieces directly from the board here
    // and keep feature extraction to just the more complicated functions
    float evaluate(){
        // extract features from the board
        FeatureExtractor fe = FeatureExtractor(board);
        fe.extract();
        Features features = fe.getFeatures();

        float gamePhase = 1; // completely middlegame
        // create a value to represent how much of the endgame score we should use
        if (features.endgameScore < 24){
            gamePhase = 0; // completely endgame
        }
        else{
            gamePhase = (features.endgameScore - 24) / 24; // somewhere in between
            //normalize gamePhase to be between 0 and 1
            if (gamePhase > 1){
                gamePhase = 1;
            }
        }

        float mgWeight = gamePhase;
        float egWeight = 1 - gamePhase;

        float score = 0;
        //pawn evaluation
        for (int i = 0; i < features.wpawns.size(); i++){
            score += weightFeaturesByHand.pawnsMG[features.wpawns[i]] * mgWeight; 
            score += weightFeaturesByHand.pawnsEG[features.wpawns[i]] * egWeight;
        }
        for (int i = 0; i < features.bpawns.size(); i++){
            score -= weightFeaturesByHand.pawnsMG[features.bpawns[i]] * mgWeight;
            score -= weightFeaturesByHand.pawnsEG[features.bpawns[i]] * egWeight;
        }
        //knight evaluation
        for (int i = 0; i < features.wknights.size(); i++){
            score += weightFeaturesByHand.knightsMG[features.wknights[i]] * mgWeight;
            score += weightFeaturesByHand.knightsEG[features.wknights[i]] * egWeight;
        }
        for (int i = 0; i < features.bknights.size(); i++){
            score -= weightFeaturesByHand.knightsMG[features.bknights[i]] * mgWeight;
            score -= weightFeaturesByHand.knightsEG[features.bknights[i]] * egWeight;
        }
        //bishop evaluation
        for (int i = 0; i < features.wbishops.size(); i++){
            score += weightFeaturesByHand.bishopsMG[features.wbishops[i]] * mgWeight;
            score += weightFeaturesByHand.bishopsEG[features.wbishops[i]] * egWeight;
        }
        for (int i = 0; i < features.bbishops.size(); i++){
            score -= weightFeaturesByHand.bishopsMG[features.bbishops[i]] * mgWeight;
            score -= weightFeaturesByHand.bishopsEG[features.bbishops[i]] * egWeight;
        }
        //rook evaluation
        for (int i = 0; i < features.wrooks.size(); i++){
            score += weightFeaturesByHand.rooksMG[features.wrooks[i]] * mgWeight;
            score += weightFeaturesByHand.rooksEG[features.wrooks[i]] * egWeight;
        }
        for (int i = 0; i < features.brooks.size(); i++){
            score -= weightFeaturesByHand.rooksMG[features.brooks[i]] * mgWeight;
            score -= weightFeaturesByHand.rooksEG[features.brooks[i]] * egWeight;
        }
        //queen evaluation
        for (int i = 0; i < features.wqueen.size(); i++){
            score += weightFeaturesByHand.queensMG[features.wqueen[i]] * mgWeight;
            score += weightFeaturesByHand.queensEG[features.wqueen[i]] * egWeight;
        }
        for (int i = 0; i < features.bqueen.size(); i++){
            score -= weightFeaturesByHand.queensMG[features.bqueen[i]] * mgWeight;
            score -= weightFeaturesByHand.queensEG[features.bqueen[i]] * egWeight;
        }
        //king evaluation
        for (int i = 0; i < features.wking.size(); i++){
            score += weightFeaturesByHand.kingsMG[features.wking[i]] * mgWeight;
            score += weightFeaturesByHand.kingsEG[features.wking[i]] * egWeight;
        }
        for (int i = 0; i < features.bking.size(); i++){
            score -= weightFeaturesByHand.kingsMG[features.bking[i]] * mgWeight;
            score -= weightFeaturesByHand.kingsEG[features.bking[i]] * egWeight;
        }
        //passed pawn evaluation
        score += features.passedPawns * weightFeaturesByHand.passedPawn.middleGame * mgWeight;
        score += features.passedPawns * weightFeaturesByHand.passedPawn.endGame * egWeight;

        //doubled pawn evaluation
        score += features.doubledPawns * weightFeaturesByHand.doubledPawn.middleGame * mgWeight;
        score += features.doubledPawns * weightFeaturesByHand.doubledPawn.endGame * egWeight;

        //isolated pawn evaluation
        score += features.isolatedPawns * weightFeaturesByHand.isolatedPawn.middleGame * mgWeight;
        score += features.isolatedPawns * weightFeaturesByHand.isolatedPawn.endGame * egWeight;

        //weak pawn evaluation
        score += features.weakPawns * weightFeaturesByHand.weakPawn.middleGame * mgWeight;
        score += features.weakPawns * weightFeaturesByHand.weakPawn.endGame * egWeight;

        //weak square evaluation
        score += features.weakSquares * weightFeaturesByHand.weakSquare.middleGame * mgWeight;
        score += features.weakSquares * weightFeaturesByHand.weakSquare.endGame * egWeight;

        //passed pawn enemy king square evaluation
        score += features.passedPawnEnemyKingSquare * weightFeaturesByHand.passedPawnEnemyKingSquare.middleGame * mgWeight;
        score += features.passedPawnEnemyKingSquare * weightFeaturesByHand.passedPawnEnemyKingSquare.endGame * egWeight;

        //knight outpost evaluation
        score += features.knightOutposts * weightFeaturesByHand.knightOutposts.middleGame * mgWeight;
        score += features.knightOutposts * weightFeaturesByHand.knightOutposts.endGame * egWeight;

        //bishop mobility evaluation
        score += features.bishopMobility * weightFeaturesByHand.bishopMobility.middleGame * mgWeight;
        score += features.bishopMobility * weightFeaturesByHand.bishopMobility.endGame * egWeight;

        //bishop pair evaluation
        score += features.bishopPair * weightFeaturesByHand.bishopPair.middleGame * mgWeight;
        score += features.bishopPair * weightFeaturesByHand.bishopPair.endGame * egWeight;

        //rook attack king file evaluation
        score += features.rookAttackKingFile * weightFeaturesByHand.rookAttackKingFile.middleGame * mgWeight;
        score += features.rookAttackKingFile * weightFeaturesByHand.rookAttackKingFile.endGame * egWeight;

        //rook attack king adjacent file evaluation
        score += features.rookAttackKingAdjFile * weightFeaturesByHand.rookAttackKingAdjFile.middleGame * mgWeight;
        score += features.rookAttackKingAdjFile * weightFeaturesByHand.rookAttackKingAdjFile.endGame * egWeight;

        //rook 7th rank evaluation
        score += features.rook7thRank * weightFeaturesByHand.rook7thRank.middleGame * mgWeight;
        score += features.rook7thRank * weightFeaturesByHand.rook7thRank.endGame * egWeight;

        //rook connected evaluation
        score += features.rookConnected * weightFeaturesByHand.rookConnected.middleGame * mgWeight;
        score += features.rookConnected * weightFeaturesByHand.rookConnected.endGame * egWeight;

        //rook mobility evaluation
        score += features.rookMobility * weightFeaturesByHand.rookMobility.middleGame * mgWeight;
        score += features.rookMobility * weightFeaturesByHand.rookMobility.endGame * egWeight;

        //rook behind passed pawn evaluation
        score += features.rookBehindPassedPawn * weightFeaturesByHand.rookBehindPassedPawn.middleGame * mgWeight;
        score += features.rookBehindPassedPawn * weightFeaturesByHand.rookBehindPassedPawn.endGame * egWeight;

        //rook open file evaluation
        score += features.rookOpenFile * weightFeaturesByHand.rookOpenFile.middleGame * mgWeight;
        score += features.rookOpenFile * weightFeaturesByHand.rookOpenFile.endGame * egWeight;

        //rook semi open file evaluation
        score += features.rookSemiOpenFile * weightFeaturesByHand.rookSemiOpenFile.middleGame * mgWeight;
        score += features.rookSemiOpenFile * weightFeaturesByHand.rookSemiOpenFile.endGame * egWeight;

        //rook attack weak pawn open column evaluation
        score += features.rookAtckWeakPawnOpenColumn * weightFeaturesByHand.rookAtckWeakPawnOpenColumn.middleGame * mgWeight;
        score += features.rookAtckWeakPawnOpenColumn * weightFeaturesByHand.rookAtckWeakPawnOpenColumn.endGame * egWeight;

        //king friendly pawn evaluation
        score += features.kingFriendlyPawn * weightFeaturesByHand.kingFriendlyPawn.middleGame * mgWeight;
        score += features.kingFriendlyPawn * weightFeaturesByHand.kingFriendlyPawn.endGame * egWeight;

        //king no enemy pawn near evaluation
        score += features.kingNoEnemyPawnNear * weightFeaturesByHand.kingNoEnemyPawnNear.middleGame * mgWeight;
        score += features.kingNoEnemyPawnNear * weightFeaturesByHand.kingNoEnemyPawnNear.endGame * egWeight;

        //king pressure score evaluation
        score += features.kingPressureScore * weightFeaturesByHand.kingPressureScore.middleGame * mgWeight;
        score += features.kingPressureScore * weightFeaturesByHand.kingPressureScore.endGame * egWeight;

        
        return score;
    }
};