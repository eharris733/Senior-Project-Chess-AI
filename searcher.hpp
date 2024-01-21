#include "chess.hpp"
#include "evaluator.hpp"
#include <cstdlib>
#include <ctime>

using namespace chess;

class Searcher {
public:
    Searcher(Board& initialBoard) : board(initialBoard), evaluator(initialBoard) {}

    int negamax(int depth) {
        if (depth == 0 || GameResult::NONE != board.isGameOver().second) {
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

        return bestScore;
    }

private:
    Board& board;
    Evaluator evaluator;

    int evaluate() {
        return evaluator.evaluate();
    }
};
    

