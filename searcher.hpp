#pragma once
#include "chess.hpp"
#include "evaluator.hpp"
#include "t_table.hpp"
#include <climits> // For INT_MIN and INT_MAX
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <chrono> // for timer

using namespace chess;

// Struct to hold the results of a search
struct SearchResult {
    Move bestMove;
    Move bestMove2; // last depth's best move
    int depth;
    int score;
    int nodes;
};

// variable that is called when the engine is told to stop searching
// will also be used for time control
extern std::atomic<bool> stop;

class Searcher {
public:
    Searcher(Board& initialBoard) : board(initialBoard), evaluator(initialBoard), tt(1 << 20){
    }
    SearchResult search(int timeRemaining, int timeIncrement, int movesToGo) {
        // crude time control
        // we divide by 2 because the last depth is probably going to be more than 5 times as long as the rest
        int timeForThisMove = (timeRemaining / movesToGo + timeIncrement) / 5;

        // start the timer
        auto startTime = chrono::steady_clock::now();

        result.bestMove = Move::NULL_MOVE;
        result.bestMove2 = Move::NULL_MOVE;
        result.depth = 2;
        result.score = INT_MIN + 1;
        result.nodes = 0;

        // clear the table for every search
        tt.clear();

        // while we haven't been told to stop, and we haven't reached the desired think time
        while (!stop.load()) {
            int bestScore = result.score;
            int alpha = result.score;
            int beta = INT_MAX;

            // check to see if we have reached the desired think time
            auto currentTime = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(currentTime - startTime).count();

            if (elapsed >= timeForThisMove) {
                stop = true; // Signal to stop the search
                break;
            }

            Movelist moves;
            movegen::legalmoves<MoveGenType::ALL>(moves, board);
            sortMoves(moves, board, result); // we don't have a pv yet

            for (const Move& move : moves) {
                if (stop.load()) break; // Check for stop signal

                board.makeMove(move);
                int eval = -negamax(result.depth, -beta, -alpha, true); // isroot = true
                board.unmakeMove(move);

                if (eval > bestScore) {
                    bestScore = eval;
                    result.bestMove = move;
                    result.score = bestScore; // Update the score in result
                }

                alpha = std::max(alpha, eval);
                if (alpha >= beta) {
                    break; // Alpha-beta pruning
                }
            }
            cout << " info depth " << result.depth << " score cp " << result.score << " pv " << uci::moveToUci(result.bestMove) << " nodes " << result.nodes << endl;
            result.depth++; // Update the depth after each iteration
        }

        stop = false; // Reset the stop signal
        return result; // Return the search result
    }

private:
    Board& board; // The board to search on
    Evaluator evaluator; // our evaluation function
    TranspositionTable tt; // Transposition table

    // keep track of moves
    SearchResult result;

    NodeType determineNodeType(float bestScore, float alpha, float beta) {
        if (bestScore <= alpha) {
            return NodeType::UPPERBOUND; // Failed low, this is a beta node
        } else if (bestScore >= beta) {
            return NodeType::LOWERBOUND; // Failed high, this is an alpha node
        } else {
            return NodeType::EXACT; // Exact score
        }
    }

    int negamax(int depth, int alpha, int beta, bool isRoot = false) {
        if (board.isRepetition() || board.isInsufficientMaterial() || board.isHalfMoveDraw()) {
            return 0; // Draw score
        }

        uint64_t zobristKey = board.zobrist();
        auto ttEntry = tt.retrieve(zobristKey);

        if (ttEntry.has_value() && ttEntry->depth >= depth) {
            if ((ttEntry->nodeType == NodeType::EXACT) ||
                (ttEntry->nodeType == NodeType::LOWERBOUND && ttEntry->score > alpha) ||
                (ttEntry->nodeType == NodeType::UPPERBOUND && ttEntry->score < beta)) {
                // If this is the root node and we have a best move stored, update result.bestMove.
                if (isRoot && ttEntry->bestMove != Move::NULL_MOVE) {
                    result.bestMove = ttEntry->bestMove;
                }
                return ttEntry->score; // Use the score from the transposition table.
            }
        }

        int bestScore = INT_MIN + 1;
        Move localBestMove = Move::NULL_MOVE;
        Movelist moves;
        movegen::legalmoves<MoveGenType::ALL>(moves, board);

        if (moves.size() == 0) {
            // Check for checkmate or stalemate
            return board.inCheck() ? (-100000 - depth) : 0;
        }

        if (depth == 0) {
            return evaluate(depth); // Leaf node evaluation
        }

        sortMoves(moves, board, result); // Pre-sort moves based on heuristics

        for (const Move& move : moves) {
            board.makeMove(move);
            int score = -negamax(depth - 1, -beta, -alpha, false); // Recurse with flipped bounds and not root.
            board.unmakeMove(move);

            if (score > bestScore) {
                bestScore = score;
                localBestMove = move; // Found a new best move.
            }

            alpha = std::max(alpha, score);
            if (alpha >= beta) {
                break; // Alpha-beta cutoff.
            }
        }

        // Update the transposition table with the new best score and move found at this depth.
        tt.save(zobristKey, depth, bestScore, determineNodeType(bestScore, alpha, beta), localBestMove);

        return bestScore;
    }

    // sorts in order of 
    // PV, then captures, then everything else
    void sortMoves(Movelist& moves, Board& board, SearchResult& result) {
        std::sort(moves.begin(), moves.end(), [&board, &result](const Move& a, const Move& b) -> bool {
            if (result.bestMove != Move::NULL_MOVE) {
                if (a == result.bestMove) {
                    return true;
                } else if (b == result.bestMove) {
                    return false;
                }
            }

            bool aIsCapture = board.isCapture(a);
            bool bIsCapture = board.isCapture(b);
            return aIsCapture && !bIsCapture; // Prioritize captures over non-captures
        });
        // May want to add other logic here later
    }

    int evaluate(int depth) {
        int rawScore = evaluator.evaluate(depth); // Positive for White's advantage, negative for Black's
        return board.sideToMove() == Color::WHITE ? rawScore : -rawScore;
    }
};
