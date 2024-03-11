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

TunableEval result1 = {
    "",
    GamePhaseValue(100, 127), // Pawn
    GamePhaseValue(386, 317), // Knight
    GamePhaseValue(371, 366), // Bishop
    GamePhaseValue(516, 554), // Rook
    GamePhaseValue(816, 955), // Queen
    GamePhaseValue(33, 55), // Passed pawn
    GamePhaseValue(63, 25), // Doubled pawn (not good)
    GamePhaseValue(35, 29), // Isolated pawn (not good)
    GamePhaseValue(12, 2), // Weak pawn
    GamePhaseValue(13, 10), // Central pawn
    GamePhaseValue(6, 10), // Weak square
    GamePhaseValue(15, 4), // Passed pawn enemy king square
    GamePhaseValue(21, 59), // Knight on outpost (knightOutposts)
    GamePhaseValue(5, 6), // Knight mobility (knightMobility)
    GamePhaseValue(10, 6), // Bishop mobility (bishopMobility)
    GamePhaseValue(56, 9), // Possessing both colored bishops (bishopPair)
    GamePhaseValue(57, 31), // Rook is on same file as enemy king (rookAttackKingFile)
    GamePhaseValue(41, 15), // Rook is on adjacent file to enemy king (rookAttackKingAdjFile)
    GamePhaseValue(23, 48), // Rook is on second to last rank (rook7thRank)
    GamePhaseValue(62, 24), // Rooks can guard each other (rookConnected)
    GamePhaseValue(5, 2), // Rook mobility (rookMobility)
    GamePhaseValue(8, 51), // Rook is behind a passed pawn (rookBehindPassedPawn)
    GamePhaseValue(50, 38), // Rook has no pawns in its file (rookOpenFile)
    GamePhaseValue(33, 18), // Rook has no friendly pawns in its file (rookSemiOpenFile)
    GamePhaseValue(62, 18), // Rook can directly attack a weak enemy pawn (rookAtckWeakPawnOpenColumn)
    GamePhaseValue(2, 2), // Queen mobility (queenMobility)
    GamePhaseValue(14, 1), // King friendly pawn closeness (kingFriendlyPawn)
    GamePhaseValue(14, 1), // King distance from enemy pawns (kingNoEnemyPawnNear)
    242, // maxKingSafetyScore
    1, // steepnessKingSafetyScore
    8, // middlePointKingSafetyScore
};

TunableEval result2 = {
    "",
    GamePhaseValue(100, 123), // Pawn
    GamePhaseValue(375, 275), // Knight
    GamePhaseValue(385, 254), // Bishop
    GamePhaseValue(519, 657), // Rook
    GamePhaseValue(957, 953), // Queen
    GamePhaseValue(7, 46), // Passed pawn
    GamePhaseValue(54, 38), // Doubled pawn (not good)
    GamePhaseValue(28, 2), // Isolated pawn (not good)
    GamePhaseValue(3, 18), // Weak pawn
    GamePhaseValue(15, 11), // Central pawn
    GamePhaseValue(0, 2), // Weak square
    GamePhaseValue(5, 23), // Passed pawn enemy king square
    GamePhaseValue(3, 33), // Knight on outpost (knightOutposts)
    GamePhaseValue(12, 17), // Knight mobility (knightMobility)
    GamePhaseValue(7, 19), // Bishop mobility (bishopMobility)
    GamePhaseValue(48, 6), // Possessing both colored bishops (bishopPair)
    GamePhaseValue(9, 33), // Rook is on same file as enemy king (rookAttackKingFile)
    GamePhaseValue(18, 18), // Rook is on adjacent file to enemy king (rookAttackKingAdjFile)
    GamePhaseValue(23, 52), // Rook is on second to last rank (rook7thRank)
    GamePhaseValue(24, 16), // Rooks can guard each other (rookConnected)
    GamePhaseValue(0, 0), // Rook mobility (rookMobility)
    GamePhaseValue(50, 4), // Rook is behind a passed pawn (rookBehindPassedPawn)
    GamePhaseValue(35, 19), // Rook has no pawns in its file (rookOpenFile)
    GamePhaseValue(19, 40), // Rook has no friendly pawns in its file (rookSemiOpenFile)
    GamePhaseValue(58, 17), // Rook can directly attack a weak enemy pawn (rookAtckWeakPawnOpenColumn)
    GamePhaseValue(2, 0), // Queen mobility (queenMobility)
    GamePhaseValue(15, 10), // King friendly pawn closeness (kingFriendlyPawn)
    GamePhaseValue(9, 0), // King distance from enemy pawns (kingNoEnemyPawnNear)
    326, // maxKingSafetyScore
    14, // steepnessKingSafetyScore
    53, // middlePointKingSafetyScore
};


TunableEval result3 = {
    chess::constants::STARTPOS,
    GamePhaseValue(100, 100), // Pawn
    GamePhaseValue(334, 300), // Knight
    GamePhaseValue(187, 320), // Bishop
    GamePhaseValue(480, 520), // Rook
    GamePhaseValue(624, 910), // Queen
    GamePhaseValue(39, 31), // Passed Pawn
    GamePhaseValue(10, 23), // Doubled Pawn
    GamePhaseValue(9, 23), // Isolated Pawn
    GamePhaseValue(39, 40), // Weak Pawn
    GamePhaseValue(10, 21), // Central Pawn
    GamePhaseValue(9, 29), // Weak Square
    GamePhaseValue(0, 50), // Passed Pawn Enemy King Square
    GamePhaseValue(39, 35), // Knight Outposts
    GamePhaseValue(1, 13), // Knight Mobility
    GamePhaseValue(12, 14), // Bishop Mobility
    GamePhaseValue(15, 20), // Bishop Pair
    GamePhaseValue(15, 4), // Rook Attack King File
    GamePhaseValue(10, 5), // Rook Attack King Adjacent File
    GamePhaseValue(25, 35), // Rook 7th Rank
    GamePhaseValue(21, 10), // Rook Connected
    GamePhaseValue(10, 1), // Rook Mobility
    GamePhaseValue(10, 35), // Rook Behind Passed Pawn
    GamePhaseValue(7, 26), // Rook Open File
    GamePhaseValue(20, 8), // Rook Semi Open File
    GamePhaseValue(60, 30), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(0, 14), // King Friendly Pawn
    GamePhaseValue(4, 15), // King No Enemy Pawn Near
    GamePhaseValue(0, 5), // Queen Mobility
    500, // maxKingSafetyScore
    3, // steepnessKingSafetyScore (will be divided by 100)
    38, // middlePointKingSafetyScore
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