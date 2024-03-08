#include "chess.hpp"
#pragma once

// this file defines the structs that I will use in my program, as well as gets the baseline versions of them
// baselines should play at about a 2400 strength

// eval function weights
// a struct to allow each feature to be weighted differently based on phase of the game
struct GamePhaseValue {
    int middleGame;
    int endGame;

    GamePhaseValue(int mg = 0, int eg = 0) : middleGame(mg), endGame(eg) {}
};

// define the struct here
struct TunableEval{
    std::string fen; // The FEN string
    GamePhaseValue pawn;
    GamePhaseValue knight;
    GamePhaseValue bishop;
    GamePhaseValue rook;
    GamePhaseValue queen;
    GamePhaseValue passedPawn;
    GamePhaseValue doubledPawn;
    GamePhaseValue isolatedPawn;
    GamePhaseValue weakPawn;
    GamePhaseValue centralPawn;
    GamePhaseValue weakSquare;
    GamePhaseValue passedPawnEnemyKingSquare;
    GamePhaseValue knightOutposts;
    GamePhaseValue knightMobility;
    GamePhaseValue bishopMobility;
    GamePhaseValue bishopPair;
    GamePhaseValue rookAttackKingFile;
    GamePhaseValue rookAttackKingAdjFile;
    GamePhaseValue rook7thRank;
    GamePhaseValue rookConnected;
    GamePhaseValue rookMobility;
    GamePhaseValue rookBehindPassedPawn;
    GamePhaseValue rookOpenFile;
    GamePhaseValue rookSemiOpenFile;
    GamePhaseValue rookAtckWeakPawnOpenColumn;
    GamePhaseValue queenMobility;
    GamePhaseValue kingFriendlyPawn;
    GamePhaseValue kingNoEnemyPawnNear;
    int maxKingSafetyScore;
    int steepnessKingSafetyScore;
    int middlePointKingSafetyScore;
};

struct TunableSearch{
    //tuneable search parameters

    // for aspiration window
    int aspirationWindowProgression[2];
    int aspirationWindowInitialDelta;
    int useAspirationWindowDepth;

    // for null move reductions
    bool useLazyEvalNMR;

    // for futility pruning
    int futilityMargin[3];
    bool useLazyEvalFutility;
    // for delta pruning in QS search
    int deltaMargin;

    // for move ordering
    int promotionMoveScore;
    int killerMoveScore;
    int baseScore;

    //for late move reductions
    int initalDepthLMR;
    int secondaryDepthLMR;
    int initialMoveCountLMR;
    int secondaryMoveCountLMR;
};

// Global instance
TunableEval baseEval = {
    chess::constants::STARTPOS,
    //piece values
    GamePhaseValue(100, 100), // Pawn
    GamePhaseValue(305, 300), // Knight
    GamePhaseValue(315, 320), // Bishop
    GamePhaseValue(480, 520), // Rook
    GamePhaseValue(910, 910), // Queen

    //values are completely guessed based on whims
    GamePhaseValue(40, 80), // Passed pawn
    GamePhaseValue(10, 20), // Doubled pawn (not good)
    GamePhaseValue(10, 20), // Isolated pawn (not good)
    GamePhaseValue(40, 40), // Weak pawn
    GamePhaseValue(10, 10), // Central pawn
    GamePhaseValue(5, 2), // Weak square
    GamePhaseValue(0, 50), // Rule of the square
    GamePhaseValue(40, 35), // Knight is on a weak square
    GamePhaseValue(1, 2), // How many squares a knight can move to
    GamePhaseValue(3, 1), // How many squares a bishop can move to
    GamePhaseValue(15, 20), // Possessing both colored bishops
    GamePhaseValue(15, 5), // Rook is on same file as enemy king
    GamePhaseValue(10, 5), // Rook is on adjacent file to enemy king
    GamePhaseValue(25, 35), // Rook is on second to last rank
    GamePhaseValue(20, 10), // Rooks can guard each other
    GamePhaseValue(2, 1), // How many squares a rook has
    GamePhaseValue(10, 35), // Rook is behind a passed pawn
    GamePhaseValue(15, 5), // Rook has no pawns in its file
    GamePhaseValue(20, 10), // Rook has no friendly pawns in its file
    GamePhaseValue(35, 30), // Rook can directly attack a weak enemy pawn
    GamePhaseValue(1, 1), // How many squares a queen can move to
    GamePhaseValue(4, 0), // How many and how close are friendly pawns (closeness * numberofpawns / numberofpawns)
    GamePhaseValue(5, 0), // How far king is from closest enemy pawn// taken from the chess programming wiki's scaled version from stockfish
    500, // maxKingSafetyScore
    4, // steepnessKingSafetyScore (will be divided by 100)
    25, // middlePointKingSafetyScore

    // {
    //   0,  0,   1,   2,   3,   5,   7,   9,  12,  15,
    //   18,  22,  26,  30,  35,  39,  44,  50,  56,  62,
    //   68,  75,  82,  85,  89,  97, 105, 113, 122, 131,
    //   140, 150, 169, 180, 191, 202, 213, 225, 237, 248,
    //   260, 272, 283, 295, 307, 319, 330, 342, 354, 366,
    //   377, 389, 401, 412, 424, 436, 448, 459, 471, 483,
    //   494, 500,
    // }, // King safety table more or less how it will be
};

TunableEval zeroEval = {
    "",
    GamePhaseValue(0, 0), // Pawn
    GamePhaseValue(0, 0), // Knight
    GamePhaseValue(0, 0), // Bishop
    GamePhaseValue(0, 0), // Rook
    GamePhaseValue(0, 0), // Queen
    GamePhaseValue(0, 0), // Passed pawn
    GamePhaseValue(0, 0), // Doubled pawn (not good)
    GamePhaseValue(0, 0), // Isolated pawn (not good)
    GamePhaseValue(0, 0), // Weak pawn
    GamePhaseValue(0, 0), // Central pawn
    GamePhaseValue(0, 0), // Weak square
    GamePhaseValue(0, 0), // Rule of the square
    GamePhaseValue(0, 0), // Knight is on a weak square
    GamePhaseValue(0, 0), // How many squares a knight can move to
    GamePhaseValue(0, 0), // How many squares a bishop can move to
    GamePhaseValue(0, 0), // Possessing both colored bishops
    GamePhaseValue(0, 0), // Rook is on same file as enemy king
    GamePhaseValue(0, 0), // Rook is on adjacent file to enemy king
    GamePhaseValue(0, 0), // Rook is on second to last rank
    GamePhaseValue(0, 0), // Rooks can guard each other
    GamePhaseValue(0, 0), // How many squares a rook has
    GamePhaseValue(0, 0), // Rook is behind a passed pawn
    GamePhaseValue(0, 0), // Rook has no pawns in its file
    GamePhaseValue(0, 0), // Rook has no friendly pawns in its file
    GamePhaseValue(0, 0), // Rook can directly attack a weak enemy pawn
    GamePhaseValue(0, 0), // How many squares a queen can move to
    GamePhaseValue(0, 0), // How many and how close are friendly pawns (closeness * numberofpawns / numberofpawns)
    GamePhaseValue(0, 0), // How far king is from closest enemy pawn
    0, // maxKingSafetyScore
    0, // steepnessKingSafetyScore
    0, // middlePointKingSafetyScore
};



// this version of iterative deepening is heavily influened 
// by Sebastian Lague's chess engine tutorial
TunableSearch baseSearch = {
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
        3, // initial depth for late move reductions
        6, // secondary depth for late move reductions
        3, // initial move count for late move reductions
        4, // secondary move count for late move reductions
    };