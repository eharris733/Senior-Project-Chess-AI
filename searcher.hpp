#include "chess.hpp"
#include <cstdlib>
#include <ctime>

using namespace chess;

class Searcher {
public:
    Searcher(Board& initialBoard) : board(initialBoard) {}

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

    int evaluate() {
        // Random evaluation function for now
        srand(time(0));
        return rand() % 100;
    } 
};
    

