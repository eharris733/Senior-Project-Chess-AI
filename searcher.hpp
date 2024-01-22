#include "chess.hpp"
#include "evaluator.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace chess;

class Searcher {
public:
    Searcher(Board& initialBoard) : board(initialBoard), evaluator(initialBoard) {}
    
    //return a move object for test purposes, not needed for UCI loop
    Move search(int depth) {
        int bestScore = INT_MIN;
        Move bestMove;
        int score;
        Movelist moves;
        movegen::legalmoves(moves, board);

        for (const Move& move : moves) {
            board.makeMove(move);
            score = -negamax(depth - 1);
            board.unmakeMove(move);

            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
        }
        
        std::cout << "bestmove " << uci::moveToUci(bestMove) << std::endl;
        

        return bestMove;

    }

private:
    Board& board;
    Evaluator evaluator;

    int negamax(int depth) {
        auto gameResult = board.isGameOver().second;
         if (gameResult != GameResult::NONE) {
        if (gameResult == GameResult::WIN) {
                return 1000000 - depth;
        } else if (gameResult == GameResult::DRAW) {
            // Score for stalemate could be 0 or based on evaluation
            return 0;
        }
    }
        if (depth == 0 || GameResult::NONE != gameResult) {
                return evaluate();
        }

        int bestScore = INT_MIN;
        Movelist moves;
        movegen::legalmoves(moves, board);

        for (const Move& move : moves) {
            board.makeMove(move);
            int score = -negamax(depth - 1);
            board.unmakeMove(move);

            if (score > bestScore) {
                bestScore = score;
            }
        }

        std::cout << "info depth " << depth << " score cp " << bestScore << std::endl;

        return bestScore;
    }

    int evaluate() {
    int rawScore = evaluator.evaluate(); // Assume positive for White's advantage, negative for Black's.
    return board.sideToMove() == Color::WHITE ? rawScore : -rawScore;
}

};
