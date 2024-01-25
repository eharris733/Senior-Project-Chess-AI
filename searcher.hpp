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
        
        std::cout << "bestmove " << uci::moveToUci(bestMove) << std::endl;
        // << "info depth " << depth << " score cp " << bestScore 

        return bestMove;

    }

private:
    Board& board;
    Evaluator evaluator;

    int negamax(int depth) {
        GameResult gameResult = board.isGameOver().second;
        if (depth == 0 || GameResult::NONE != gameResult) {
                return evaluate(depth);
        }

        int bestScore = INT_MIN;
        Movelist moves;
        movegen::legalmoves(moves, board);

        for (const Move& move : moves) {
            board.makeMove(move);
            int eval = -negamax(depth - 1);
            board.unmakeMove(move);

            if (eval > bestScore) {
                bestScore = eval;
            }
        }

        return bestScore;
    }

    int evaluate(int depth) {
    int rawScore = evaluator.evaluate(depth); // Assume positive for White's advantage, negative for Black's.
    return board.sideToMove() == Color::WHITE ? rawScore : -rawScore;
}

};
