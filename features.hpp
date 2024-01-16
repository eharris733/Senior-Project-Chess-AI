#pragma once

#ifndef FEATURES_HPP
#define FEATURES_HPP

#include "chess.hpp"
#include <vector>
#include <string>

// Forward declaration of the Features structure
struct Features;

// Structure to hold expected feature values for a FEN
struct Features {
    std::string fen; // The FEN string
    std::vector<chess::Square> wpawns; // A list of all the white pawn locations
    std::vector<chess::Square> bpawns; // A list of all the black pawn locations
    std::vector<chess::Square> wknights; // A list of all the white knight locations
    std::vector<chess::Square> bknights; // A list of all the black knight locations
    std::vector<chess::Square> wbishops; // A list of all the white bishop locations
    std::vector<chess::Square> bbishops; // A list of all the black bishop locations
    std::vector<chess::Square> wrooks; // A list of all the white rook locations
    std::vector<chess::Square> brooks; // A list of all the black rook locations
    std::vector<chess::Square> wqueen; // A list of all the white queen locations
    std::vector<chess::Square> bqueen; // A list of all the black queen locations
    std::vector<chess::Square> wking; // A veector containing the one white king locations (vector for consistency)
    std::vector<chess::Square> bking; // A vector containing the one black king locations (vector for consistency)
    short passedPawns; // Pawn with no other pawns in its way
    short doubledPawns; // Pawns stacked on the same file
    short isolatedPawns; // Pawn with no adjacent pawns
    short backwardPawns; // Pawn with all adjacent pawns advanced
    short weakSquares; // Square on your side where no pawns can attack
    short passedPawnEnemyKingSquare; // Rule of the square
    short knightOutposts; // Knight is on a weak square
    short bishopMobility; // How many squares a bishop can move to
    short bishopPair; // Possessing both colored bishops
    short rookAttackKingFile; // Rook is on same file as enemy king
    short rookAttackKingAdjFile; // Rook is on adjacent file to enemy king
    short rook7thRank; // Rook is on second to last rank
    short rookConnected; // Rooks can guard each other
    short rookMobility; // How many squares a rook has
    short rookBehindPassedPawn; // Rook is behind a passed pawn
    short rookOpenFile; // Rook has no pawns in its file
    short rookSemiOpenFile; // Rook has no friendly pawns in its file
    short rookAtckWeakPawnOpenColumn; // Rook can directly attack a weak enemy pawn
    short kingFriendlyPawn; // How many and how close are friendly pawns (closeness * numberofpawns / numberofpawns)
    short kingNoEnemyPawnNear; // How far king is from closest enemy pawn
    float kingPressureScore; // Multiplier for king pressure
};

#endif // FEATURES_HPP

