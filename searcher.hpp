#include "chess.hpp"
#include <cstdlib>
#include <ctime>

using namespace chess;

class Searcher {
public:
    int negamax(Board& board, int depth) {
        if (depth == 0 || board.isGameOver()) {
            return evaluate(board);
        }

        int bestScore = INT_MIN;
        std::vector<Move> moves = board.generateLegalMoves();

        for (const Move& move : moves) {
            board.makeMove(move);
            int score = -negamax(board, depth - 1);
            board.undoMove();

            if (score > bestScore) {
                bestScore = score;
            }
        }

        return bestScore;
    }

private:
    int evaluate(const Board& board) {
        // Random evaluation function for now
        srand(time(0));
        return rand() % 100;
    }
};
