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
    GamePhaseValue kingPressureScore;
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
    GamePhaseValue(5, 0), // How far king is from closest enemy pawn// taken from the 
    GamePhaseValue(3, 1),

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
    GamePhaseValue(0, 0)
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
    GamePhaseValue(0, 0), // King pressure score
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
    GamePhaseValue(0, 0), // King pressure score
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
    GamePhaseValue(0, 0), // maxKingSafetyScore
};

TunableEval result4 = {
    chess::constants::STARTPOS,
    GamePhaseValue(100, 115), // Pawn
    GamePhaseValue(190, 281), // Knight
    GamePhaseValue(430, 370), // Bishop
    GamePhaseValue(472, 504), // Rook
    GamePhaseValue(1014, 822), // Queen
    GamePhaseValue(5, 60), // Passed Pawn
    GamePhaseValue(42, 14), // Doubled Pawn
    GamePhaseValue(33, 4), // Isolated Pawn
    GamePhaseValue(28, 60), // Weak Pawn
    GamePhaseValue(40, 4), // Central Pawn
    GamePhaseValue(3, 0), // Weak Square
    GamePhaseValue(23, 26), // Passed Pawn Enemy King Square
    GamePhaseValue(30, 6), // Knight Outposts
    GamePhaseValue(24, 0), // Knight Mobility
    GamePhaseValue(10, 11), // Bishop Mobility
    GamePhaseValue(47, 2), // Bishop Pair
    GamePhaseValue(4, 15), // Rook Attack King File
    GamePhaseValue(2, 50), // Rook Attack King Adjacent File
    GamePhaseValue(3, 32), // Rook 7th Rank
    GamePhaseValue(61, 59), // Rook Connected
    GamePhaseValue(4, 2), // Rook Mobility
    GamePhaseValue(56, 47), // Rook Behind Passed Pawn
    GamePhaseValue(48, 36), // Rook Open File
    GamePhaseValue(63, 24), // Rook Semi Open File
    GamePhaseValue(50, 20), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(9, 60), // King Friendly Pawn
    GamePhaseValue(2, 18), // King No Enemy Pawn Near
    GamePhaseValue(4, 6), // Queen Mobility
    GamePhaseValue(10, 11), // King Pressure Score
};

TunableEval result5 = {
    chess::constants::STARTPOS,
    GamePhaseValue(100, 142), // Pawn
    GamePhaseValue(297, 297), // Knight
    GamePhaseValue(313, 331), // Bishop
    GamePhaseValue(420, 454), // Rook
    GamePhaseValue(850, 997), // Queen
    GamePhaseValue(29, 4), // Passed Pawn
    GamePhaseValue(55, 9), // Doubled Pawn
    GamePhaseValue(37, 3), // Isolated Pawn
    GamePhaseValue(35, 35), // Weak Pawn
    GamePhaseValue(40, 36), // Central Pawn
    GamePhaseValue(0, 15), // Weak Square
    GamePhaseValue(5, 25), // Passed Pawn Enemy King Square
    GamePhaseValue(28, 6), // Knight Outposts
    GamePhaseValue(24, 0), // Knight Mobility
    GamePhaseValue(6, 16), // Bishop Mobility
    GamePhaseValue(38, 18), // Bishop Pair
    GamePhaseValue(28, 56), // Rook Attack King File
    GamePhaseValue(13, 29), // Rook Attack King Adjacent File
    GamePhaseValue(16, 3), // Rook 7th Rank
    GamePhaseValue(62, 0), // Rook Connected
    GamePhaseValue(8, 31), // Rook Mobility
    GamePhaseValue(53, 46), // Rook Behind Passed Pawn
    GamePhaseValue(51, 52), // Rook Open File
    GamePhaseValue(57, 38), // Rook Semi Open File
    GamePhaseValue(54, 36), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(9, 2), // King Friendly Pawn
    GamePhaseValue(6, 10), // King No Enemy Pawn Near
    GamePhaseValue(0, 6), // Queen Mobility
    GamePhaseValue(9, 8), // King Pressure Score
};


// this is from gen 70 in the only wokring GA
TunableEval result6 = {
     chess::constants::STARTPOS,
    GamePhaseValue(100, 115), // Pawn
    GamePhaseValue(174, 294), // Knight
    GamePhaseValue(373, 371), // Bishop
    GamePhaseValue(455, 504), // Rook
    GamePhaseValue(885, 822), // Queen
    GamePhaseValue(2, 7), // Passed Pawn
    GamePhaseValue(20, 49), // Doubled Pawn
    GamePhaseValue(32, 7), // Isolated Pawn
    GamePhaseValue(32, 28), // Weak Pawn
    GamePhaseValue(24, 35), // Central Pawn
    GamePhaseValue(0, 13), // Weak Square
    GamePhaseValue(4, 58), // Passed Pawn Enemy King Square
    GamePhaseValue(31, 9), // Knight Outposts
    GamePhaseValue(25, 3), // Knight Mobility
    GamePhaseValue(6, 16), // Bishop Mobility
    GamePhaseValue(33, 21), // Bishop Pair
    GamePhaseValue(60, 7), // Rook Attack King File
    GamePhaseValue(3, 13), // Rook Attack King Adjacent File
    GamePhaseValue(3, 0), // Rook 7th Rank
    GamePhaseValue(2, 61), // Rook Connected
    GamePhaseValue(8, 3), // Rook Mobility
    GamePhaseValue(41, 32), // Rook Behind Passed Pawn
    GamePhaseValue(60, 34), // Rook Open File
    GamePhaseValue(41, 38), // Rook Semi Open File
    GamePhaseValue(38, 43), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(6, 3), // King Friendly Pawn
    GamePhaseValue(5, 10), // King No Enemy Pawn Near
    GamePhaseValue(1, 6), // Queen Mobility
    GamePhaseValue(11, 11), // King Pressure Score
};

TunableEval result7 = {
    chess::constants::STARTPOS,
    GamePhaseValue(100, 172), // Pawn
    GamePhaseValue(362, 445), // Knight
    GamePhaseValue(354, 477), // Bishop
    GamePhaseValue(508, 800), // Rook
    GamePhaseValue(885, 987), // Queen
    GamePhaseValue(11, 6), // Passed Pawn
    GamePhaseValue(26, 24), // Doubled Pawn
    GamePhaseValue(13, 5), // Isolated Pawn
    GamePhaseValue(19, 62), // Weak Pawn
    GamePhaseValue(32, 62), // Central Pawn
    GamePhaseValue(2, 15), // Weak Square
    GamePhaseValue(0, 19), // Passed Pawn Enemy King Square
    GamePhaseValue(40, 9), // Knight Outposts
    GamePhaseValue(15, 7), // Knight Mobility
    GamePhaseValue(21, 27), // Bishop Mobility
    GamePhaseValue(21, 32), // Bishop Pair
    GamePhaseValue(30, 1), // Rook Attack King File
    GamePhaseValue(1, 6), // Rook Attack King Adjacent File
    GamePhaseValue(36, 28), // Rook 7th Rank
    GamePhaseValue(14, 29), // Rook Connected
    GamePhaseValue(14, 27), // Rook Mobility
    GamePhaseValue(48, 48), // Rook Behind Passed Pawn
    GamePhaseValue(4, 3), // Rook Open File
    GamePhaseValue(5, 9), // Rook Semi Open File
    GamePhaseValue(23, 6), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(27, 1), // King Friendly Pawn
    GamePhaseValue(12, 11), // King No Enemy Pawn Near
    GamePhaseValue(3, 4), // Queen Mobility
    GamePhaseValue(8, 15), // King Pressure Score
};

TunableEval result8 = {
    chess::constants::STARTPOS,
    GamePhaseValue(100, 172), // Pawn
    GamePhaseValue(373, 445), // Knight
    GamePhaseValue(354, 477), // Bishop
    GamePhaseValue(508, 800), // Rook
    GamePhaseValue(885, 964), // Queen
    GamePhaseValue(11, 6), // Passed Pawn
    GamePhaseValue(26, 24), // Doubled Pawn
    GamePhaseValue(13, 5), // Isolated Pawn
    GamePhaseValue(28, 62), // Weak Pawn
    GamePhaseValue(32, 62), // Central Pawn
    GamePhaseValue(2, 15), // Weak Square
    GamePhaseValue(3, 19), // Passed Pawn Enemy King Square
    GamePhaseValue(47, 54), // Knight Outposts
    GamePhaseValue(0, 8), // Knight Mobility
    GamePhaseValue(5, 29), // Bishop Mobility
    GamePhaseValue(21, 35), // Bishop Pair
    GamePhaseValue(30, 1), // Rook Attack King File
    GamePhaseValue(1, 1), // Rook Attack King Adjacent File
    GamePhaseValue(39, 35), // Rook 7th Rank
    GamePhaseValue(14, 29), // Rook Connected
    GamePhaseValue(14, 26), // Rook Mobility
    GamePhaseValue(48, 48), // Rook Behind Passed Pawn
    GamePhaseValue(4, 3), // Rook Open File
    GamePhaseValue(6, 9), // Rook Semi Open File
    GamePhaseValue(23, 5), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(27, 1), // King Friendly Pawn
    GamePhaseValue(12, 11), // King No Enemy Pawn Near
    GamePhaseValue(2, 4), // Queen Mobility
    GamePhaseValue(8, 15), // King Pressure Score
};





// this version of iterative deepening is heavily influened 
// by Sebastian Lague's chess engine tutorial
TunableSearch baseSearch = {
        //tuneable search parameters
        {100, 350}, //aspiration window progression
        20, //aspiration window initial delta
        7, // use aspiration window depth (temporarily set to 7 so GA doesn't use it)
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


    /*
    DAD SECTION
    predefined TuneEvals meant to capture various strategies
    defined by yours truly, Gerald's dad
    */

   // values from the paper
TunableEval dad1 = {
    "",
    GamePhaseValue(83, 83), // Pawn
    GamePhaseValue(322, 322), // Knight
    GamePhaseValue(323, 323), // Bishop
    GamePhaseValue(476, 476), // Rook
    GamePhaseValue(954, 954), // Queen
    GamePhaseValue(5, 5), // Passed pawn
    GamePhaseValue(21, 21), // Doubled pawn (not good)
    GamePhaseValue(10, 10), // Isolated pawn (not good)
    GamePhaseValue(3, 3), // Weak pawn
    GamePhaseValue(0, 0), // Central pawn
    GamePhaseValue(7, 7), // Weak square
    GamePhaseValue(5, 5), // Passed pawn enemy king square
    GamePhaseValue(15, 15), // Knight on outpost (knightOutposts)
    GamePhaseValue(0, 0), // Knight mobility (knightMobility)
    GamePhaseValue(5, 5), // Bishop mobility (bishopMobility)
    GamePhaseValue(44, 44), // Possessing both colored bishops (bishopPair)
    GamePhaseValue(30, 30), // Rook is on same file as enemy king (rookAttackKingFile)
    GamePhaseValue(1, 1), // Rook is on adjacent file to enemy king (rookAttackKingAdjFile)
    GamePhaseValue(32, 32), // Rook is on second to last rank (rook7thRank)
    GamePhaseValue(2, 2), // Rooks can guard each other (rookConnected)
    GamePhaseValue(2, 2), // Rook mobility (rookMobility)
    GamePhaseValue(48, 48), // Rook is behind a passed pawn (rookBehindPassedPawn)
    GamePhaseValue(12, 12), // Rook has no pawns in its file (rookOpenFile)
    GamePhaseValue(6, 6), // Rook has no friendly pawns in its file (rookSemiOpenFile)
    GamePhaseValue(7, 7), // Rook can directly attack a weak enemy pawn (rookAtckWeakPawnOpenColumn)
    GamePhaseValue(0, 0), // Queen mobility (queenMobility)
    GamePhaseValue(27, 27), // King friendly pawn closeness (kingFriendlyPawn)
    GamePhaseValue(11, 11), // King distance from enemy pawns (kingNoEnemyPawnNear)
    GamePhaseValue(8, 8)
};
// values from my first ga run
TunableEval dad2 = {
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
    GamePhaseValue(14, 1), // King distance from enemy pawns 
    GamePhaseValue(3, 2)
};
// results from ga 2
TunableEval dad3 = {
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
    GamePhaseValue(9, 0), // King distance from enemy pawns 
    GamePhaseValue(5, 4)
};
// results from ga 3 (depth 1)
TunableEval dad4 = {
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
    GamePhaseValue(11, 10)
};

// super "by the book" values, no change in middle or endgame
TunableEval dad5 = {
    chess::constants::STARTPOS,
    GamePhaseValue(100, 100), // Pawn
    GamePhaseValue(300, 300), // Knight
    GamePhaseValue(300, 300), // Bishop
    GamePhaseValue(500, 500), // Rook
    GamePhaseValue(900, 900), // Queen
    GamePhaseValue(40, 40), // Passed Pawn
    GamePhaseValue(10, 20), // Doubled Pawn
    GamePhaseValue(10, 10), // Isolated Pawn
    GamePhaseValue(40, 40), // Weak Pawn
    GamePhaseValue(10, 10), // Central Pawn
    GamePhaseValue(5, 5), // Weak Square
    GamePhaseValue(14, 14), // Passed Pawn Enemy King Square
    GamePhaseValue(30, 30), // Knight Outposts
    GamePhaseValue(3, 3), // Knight Mobility
    GamePhaseValue(3, 3), // Bishop Mobility
    GamePhaseValue(30, 30), // Bishop Pair
    GamePhaseValue(10, 10), // Rook Attack King File
    GamePhaseValue(10, 10), // Rook Attack King Adjacent File
    GamePhaseValue(25, 25), // Rook 7th Rank
    GamePhaseValue(20, 20), // Rook Connected
    GamePhaseValue(3, 3), // Rook Mobility
    GamePhaseValue(10, 10), // Rook Behind Passed Pawn
    GamePhaseValue(10, 10), // Rook Open File
    GamePhaseValue(10, 10), // Rook Semi Open File
    GamePhaseValue(10, 10), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(3, 3), // Queen Mobility
    GamePhaseValue(3, 3), // King Friendly Pawn
    GamePhaseValue(3, 3), // King No Enemy Pawn Near
    GamePhaseValue(3, 2)
};

// non-materialistic, super aggresive
TunableEval dad6 = {
    "",
    GamePhaseValue(90, 90), // Pawn
    GamePhaseValue(250, 250), // Knight
    GamePhaseValue(250, 250), // Bishop
    GamePhaseValue(400, 400), // Rook
    GamePhaseValue(850, 850), // Queen
    GamePhaseValue(40, 40), // Passed Pawn
    GamePhaseValue(0, 0), // Doubled Pawn
    GamePhaseValue(0, 0), // Isolated Pawn
    GamePhaseValue(0, 0), // Weak Pawn
    GamePhaseValue(20, 20), // Central Pawn
    GamePhaseValue(5, 5), // Weak Square
    GamePhaseValue(0, 0), // Passed Pawn Enemy King Square
    GamePhaseValue(30, 30), // Knight Outposts
    GamePhaseValue(10, 10), // Knight Mobility
    GamePhaseValue(10, 10), // Bishop Mobility
    GamePhaseValue(30, 30), // Bishop Pair
    GamePhaseValue(50, 50), // Rook Attack King File
    GamePhaseValue(50, 50), // Rook Attack King Adjacent File
    GamePhaseValue(25, 25), // Rook 7th Rank
    GamePhaseValue(20, 20), // Rook Connected
    GamePhaseValue(10, 10), // Rook Mobility
    GamePhaseValue(10, 10), // Rook Behind Passed Pawn
    GamePhaseValue(10, 10), // Rook Open File
    GamePhaseValue(10, 10), // Rook Semi Open File
    GamePhaseValue(10, 10), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(6, 6), // Queen Mobility
    GamePhaseValue(3, 3), // King Friendly Pawn
    GamePhaseValue(20, 20), // King No Enemy Pawn Near
    GamePhaseValue(15, 11)
};

// super conserative, prefers static advantages
TunableEval dad7  = {
    chess::constants::STARTPOS,
    GamePhaseValue(100, 120), // Pawn
    GamePhaseValue(330, 320), // Knight
    GamePhaseValue(350, 370), // Bishop
    GamePhaseValue(520, 530), // Rook
    GamePhaseValue(910, 910), // Queen
    GamePhaseValue(50, 50), // Passed Pawn
    GamePhaseValue(15, 20), // Doubled Pawn
    GamePhaseValue(30, 30), // Isolated Pawn
    GamePhaseValue(40, 40), // Weak Pawn
    GamePhaseValue(10, 10), // Central Pawn
    GamePhaseValue(5, 4), // Weak Square
    GamePhaseValue(0, 5), // Passed Pawn Enemy King Square
    GamePhaseValue(35, 40), // Knight Outposts
    GamePhaseValue(2, 2), // Knight Mobility
    GamePhaseValue(2, 1), // Bishop Mobility
    GamePhaseValue(40, 40), // Bishop Pair
    GamePhaseValue(10, 5), // Rook Attack King File
    GamePhaseValue(10,104), // Rook Attack King Adjacent File
    GamePhaseValue(10, 20), // Rook 7th Rank
    GamePhaseValue(10, 10), // Rook Connected
    GamePhaseValue(2, 1), // Rook Mobility
    GamePhaseValue(10, 15), // Rook Behind Passed Pawn
    GamePhaseValue(5, 5), // Rook Open File
    GamePhaseValue(10, 10), // Rook Semi Open File
    GamePhaseValue(35, 25), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(10, 5), // King Friendly Pawn
    GamePhaseValue(10, 0), // King No Enemy Pawn Near
    GamePhaseValue(0, 1), // Queen Mobility
    GamePhaseValue(5, 5)
};


//dad 8 is just the base eval
TunableEval dad8 = {
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
    GamePhaseValue(3, 1)

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

//dad 9 is just the base eval with every value as its middlegame value
TunableEval dad9 = {
    chess::constants::STARTPOS,
    //piece values
    GamePhaseValue(100, 100), // Pawn
    GamePhaseValue(305, 305), // Knight
    GamePhaseValue(315, 315), // Bishop
    GamePhaseValue(480, 480), // Rook
    GamePhaseValue(910, 910), // Queen

    //values are completely guessed based on whims
    GamePhaseValue(40, 40), // Passed pawn
    GamePhaseValue(10, 10), // Doubled pawn (not good)
    GamePhaseValue(10, 10), // Isolated pawn (not good)
    GamePhaseValue(40, 40), // Weak pawn
    GamePhaseValue(10, 10), // Central pawn
    GamePhaseValue(5, 5), // Weak square
    GamePhaseValue(0, 50), // Rule of the square
    GamePhaseValue(40, 40), // Knight is on a weak square
    GamePhaseValue(1, 1), // How many squares a knight can move to
    GamePhaseValue(3, 3), // How many squares a bishop can move to
    GamePhaseValue(15, 15), // Possessing both colored bishops
    GamePhaseValue(15, 15), // Rook is on same file as enemy king
    GamePhaseValue(10, 10), // Rook is on adjacent file to enemy king
    GamePhaseValue(25, 25), // Rook is on second to last rank
    GamePhaseValue(20, 20), // Rooks can guard each other
    GamePhaseValue(2, 2), // How many squares a rook has
    GamePhaseValue(10, 10), // Rook is behind a passed pawn
    GamePhaseValue(15, 15), // Rook has no pawns in its file
    GamePhaseValue(20, 20), // Rook has no friendly pawns in its file
    GamePhaseValue(35, 35), // Rook can directly attack a weak enemy pawn
    GamePhaseValue(1, 1), // How many squares a queen can move to
    GamePhaseValue(4, 4), // How many and how close are friendly pawns (closeness
    GamePhaseValue(3, 3)
};
// dad 10 is just the base eval with every value as its endgame value
TunableEval dad10 = {
    chess::constants::STARTPOS,
    //piece values
    GamePhaseValue(100, 100), // Pawn
    GamePhaseValue(300, 300), // Knight
    GamePhaseValue(320, 320), // Bishop
    GamePhaseValue(520, 520), // Rook
    GamePhaseValue(910, 910), // Queen

    //values are completely guessed based on whims
    GamePhaseValue(40, 40), // Passed pawn
    GamePhaseValue(20, 20), // Doubled pawn (not good)
    GamePhaseValue(20, 20), // Isolated pawn (not good)
    GamePhaseValue(40, 40), // Weak pawn
    GamePhaseValue(10, 10), // Central pawn
    GamePhaseValue(5, 5), // Weak square
    GamePhaseValue(50, 50), // Rule of the square
    GamePhaseValue(40, 40), // Knight is on a weak square
    GamePhaseValue(2, 2), // How many squares a knight can move to
    GamePhaseValue(1, 1), // How many squares a bishop can move to
    GamePhaseValue(20, 20), // Possessing both colored bishops
    GamePhaseValue(5, 5), // Rook is on same file as enemy king
    GamePhaseValue(10, 10), // Rook is on adjacent file to enemy king
    GamePhaseValue(25, 25), // Rook is on second to last rank
    GamePhaseValue(20, 20), // Rooks can guard each other
    GamePhaseValue(1, 1), // How many squares a rook has
    GamePhaseValue(10, 10), // Rook is behind a passed pawn
    GamePhaseValue(15, 15), // Rook has no pawns in its file
    GamePhaseValue(20, 20), // Rook has no friendly pawns in its file
    GamePhaseValue(35, 35), // Rook can directly attack a weak enemy pawn
    GamePhaseValue(1, 1), // How many squares a queen can move to
    GamePhaseValue(4, 4), // How many and how close are friendly pawns (closeness
    GamePhaseValue(1, 1)

};
