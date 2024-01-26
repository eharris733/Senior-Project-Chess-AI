#include "chess.hpp"
#include "evaluator.hpp"
#include <climits> // For INT_MIN and INT_MAX
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>
#pragma once

using namespace chess;

class BasicSearcher {
public:
    BasicSearcher(Board& initialBoard) : board(initialBoard), evaluator(initialBoard) {}

    Move search(int depth) {
        int bestScore = INT_MIN;
        Move bestMove;
        int eval;
        Movelist moves;
        movegen::legalmoves(moves, board);

        for (const Move& move : moves) {
            board.makeMove(move);
            eval = -negamax(depth - 1);
            board.unmakeMove(move);

            if (eval > bestScore) {
                bestScore = eval;
                bestMove = move;
            }
        }
        
        std::cout << "bestmove " << uci::moveToUci(bestMove) 
                  << " info depth " << depth << " score cp " << bestScore << std::endl;

        return bestMove;
    }

private:
    Board& board;
    Evaluator evaluator;
    

    int negamax(int depth) {
        // Check for draw conditions at every depth
        if (board.isRepetition() || board.isInsufficientMaterial() || board.isHalfMoveDraw()) {
            return 0; // Score representing a draw
        }

        

        int bestScore = INT_MIN;
        Movelist moves;
        movegen::legalmoves(moves, board);

        bool noMoves = moves.empty();
        if (depth == 0 || noMoves) {
            return evaluate(depth, noMoves);
        }

        for (const Move& move : moves) {
            board.makeMove(move);
            int eval = -negamax(depth - 1);
            board.unmakeMove(move);

            bestScore = std::max(bestScore, eval);
            
        }

        return bestScore;
    }

    int evaluate(int depth, bool noMoves) {
        // int rawScore = evaluator.evaluate(depth); // Positive for White's advantage, negative for Black's
        // return board.sideToMove() == Color::WHITE ? rawScore : -rawScore;
        // // for testing purposes going to return a 0 on all positions
        return 0;
    }
};
