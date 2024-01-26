#include "chess.hpp"
#include "evaluator.hpp"
#include <climits> // For INT_MIN and INT_MAX
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>

using namespace chess;

class Searcher {
public:
    Searcher(Board& initialBoard) : board(initialBoard), evaluator(initialBoard) {}

    Move search(int depth) {
        if (board.isRepetition() || board.isInsufficientMaterial() || board.isHalfMoveDraw()) {
            // Assuming Move() is a valid constructor for a 'null' move indicating a draw or no action can be taken.
            std::cout << "bestmove " << '0000' << std::endl;
            return Move();
        }

        int bestScore = INT_MIN + 1;
        int alpha = INT_MIN + 1;
        int beta = INT_MAX;
        Move bestMove;
        Movelist moves;
        movegen::legalmoves(moves, board);
        sortMoves(moves, board);

        if (moves.empty()) {
            // If there are no legal moves, directly return a 'null' move. The game state should be evaluated separately.
            return Move();
        }

        for (const Move& move : moves) {
            board.makeMove(move);
            if(uci::moveToUci(move) == "h4h2"){
                std::cout << "found move" << std::endl;
                 int x = evaluator.evaluate(0, false);
            }
            int eval = -negamax(depth - 1, -beta, -alpha);
            board.unmakeMove(move);

            if (eval > bestScore) {
                bestScore = eval;
                bestMove = move;
            }
            alpha = max(alpha, eval);
            if (alpha >= beta) {
                break;
            }
        }

        std::cout << "bestmove " << uci::moveToUci(bestMove) 
                  << " info depth " << depth << " score cp " << bestScore << std::endl;

        return bestMove; // Return the best move found
    }

private:
    Board& board;
    Evaluator evaluator;

    void sortMoves(Movelist& moves, Board& board) {
        std::sort(moves.begin(), moves.end(), [&board](const Move& a, const Move& b) -> bool {
            bool aIsCapture = board.isCapture(a);
            bool bIsCapture = board.isCapture(b);
            return aIsCapture && !bIsCapture; // Prioritize captures over non-captures
        });
        // Note: Implementing checks and other prioritizations would require additional logic here.
    }

    int negamax(int depth, int alpha, int beta) {
        if (board.isRepetition() || board.isInsufficientMaterial() || board.isHalfMoveDraw()) {
            return 0; // Draw score
        }

        

        int bestScore = INT_MIN + 1;
        Movelist moves;
        movegen::legalmoves(moves, board);
        if (depth == 0 || moves.size() == 0) {
            return evaluate(depth, moves.size() == 0); // Call evaluate without parameters, assuming it calculates the score based on the current board state
        }
        sortMoves(moves, board);

        for (const Move& move : moves) {
            board.makeMove(move);
            int eval = -negamax(depth - 1, -beta, -alpha);
            board.unmakeMove(move);

            bestScore = std::max(bestScore, eval);
            alpha = max(alpha, eval);
            if (alpha >= beta) {
                break; // Alpha-beta pruning
            }
        }

        return bestScore; // Return the alpha score as the best score achievable at this node
    }



    int evaluate(int depth, bool noMoves) {
        int rawScore = evaluator.evaluate(depth); // Positive for White's advantage, negative for Black's
        return board.sideToMove() == Color::WHITE ? rawScore : -rawScore;
        // for testing purposes going to return a 0 on all positions
        //return 0;
    }
};
