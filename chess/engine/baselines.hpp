#pragma once
#include "chess.hpp"

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
    int aspirationWindowInitialDelta = 0;
    int useAspirationWindowDepth = 0;

    // for null move reductions
    bool useLazyEvalStatic = 0;

    // for futility pruning
    int futilityMargin = 0;

    // for razoring margin
    int razoringMargin = 0;

    // for delta pruning in QS search
    int deltaMargin = 0;

    // for move ordering
    int killerMoveScore = 0;

    //for late move reductions
    int initalDepthLMR = 0;
    int initialMoveCountLMR = 0;

    // for late move pruning
    int lmpMoveCount = 0;

    // for null move pruning
    int nullMovePruningInitialReduction = 0;
    int nullMovePruningDepthFactor = 0;
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

// this version of iterative deepening is heavily influened 
// by Sebastian Lague's chess engine tutorial
TunableSearch baseSearch = {
        //tuneable search parameters
        25, //aspiration window initial delta
        5, // use aspiration window depth 
        true, // use lazy eval for static eval pruning methods
        100, // futility margin
        150, // razoring margin
        300, // delta pruning in QS search
        100, // killer move score
        3, // initial depth for late move reductions
        3, // initial move count for late move reductions
        10, // late move pruning move count
        2, // null move pruning initial reduction
        10 // null move pruning depth factor
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

