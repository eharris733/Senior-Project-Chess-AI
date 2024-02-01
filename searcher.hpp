#pragma once
#include "chess.hpp"
#include "evaluator.hpp"
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
    int depth;
    int score;
    int nodes;
};

// variable that is called when the engine is told to stop searching
// will also be used for time control
extern std::atomic<bool> stop;

class Searcher {
public:
    Searcher(Board& initialBoard) : board(initialBoard), evaluator(initialBoard){
    }
SearchResult result;
   SearchResult search(int timeRemaining, int timeIncrement, int movesToGo) {
    
    // crude time control
    int timeForThisMove = timeRemaining / movesToGo + timeIncrement;

    //start the timer
    auto startTime = chrono::steady_clock::now();
    
    result.bestMove = Move::NULL_MOVE;
    result.depth = 2;
    result.score = INT_MIN;
    result.nodes = 0;

    // while we haven't been told to stop, and we haven't reached the desired think time
    while (!stop.load()) {
        int bestScore = INT_MIN + 1;
        int alpha = INT_MIN + 1;
        int beta = INT_MAX;

        //check to see if we have reached the desired think time
        auto currentTime = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(currentTime - startTime).count();

        if (elapsed >= timeForThisMove) {
            stop = true; // Signal to stop the search
            break;
        }

        

        Movelist moves;
        movegen::legalmoves<MoveGenType::ALL>(moves, board);
        sortMoves(moves, board, result.bestMove);

        for (const Move& move : moves) {
            if (stop.load()) break; // Check for stop signal

            board.makeMove(move);
            int eval = -negamax(result.depth, -beta, -alpha, result.bestMove); // Use currentDepth correctly
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
        cout << " info depth " << result.depth << " score cp " << bestScore << " pv " << uci::moveToUci(result.bestMove) << " nodes " << result.nodes << endl;
        result.depth ++; // Update the depth after each iteration
    }

    stop = false; // Reset the stop signal
    return result; // Return the search result
}


private:
    Board& board;
    Evaluator evaluator;

    //sorts in order of 
    // PV, then captures, then everything else
    void sortMoves(Movelist& moves, Board& board, Move pv) {
        std::sort(moves.begin(), moves.end(), [&board, &pv](const Move& a, const Move& b) -> bool {
            if (pv != Move::NULL_MOVE) {
                if (a == pv) {
                    return true;
                } else if (b == pv) {
                    return false;
                }
            }

            bool aIsCapture = board.isCapture(a);
            bool bIsCapture = board.isCapture(b);
            return aIsCapture && !bIsCapture; // Prioritize captures over non-captures
        });
        // May want to add other logic here later
    }

    int negamax(int depth, int alpha, int beta, Move pv) {
        // if stop command, returning 0 is risky, will have to fix this later
        if (board.isRepetition() || board.isInsufficientMaterial() || board.isHalfMoveDraw() || stop.load()) {
            return 0; // Draw score
        }

        int bestScore = INT_MIN + 1;
        Movelist moves;
        movegen::legalmoves<MoveGenType::ALL>(moves, board);
        if (depth == 0 || moves.size() == 0) {
            return evaluate(depth, moves.size() == 0); // Call evaluate without parameters, assuming it calculates the score based on the current board state
        }
        sortMoves(moves, board, pv);

        for (const Move& move : moves) {
            board.makeMove(move);
            int eval = -negamax(depth - 1, -beta, -alpha, pv);
            board.unmakeMove(move);

            bestScore = std::max(bestScore, eval);
            alpha = std::max(alpha, eval);
            if (alpha >= beta) {
                break; // Alpha-beta pruning
            }
        }

        result.nodes++; // Assuming this counts processed moves

        return bestScore; // Return the alpha score as the best score achievable at this node
    }

    int evaluate(int depth, bool noMoves) {
        int rawScore = evaluator.evaluate(depth, noMoves); // Positive for White's advantage, negative for Black's
        return board.sideToMove() == Color::WHITE ? rawScore : -rawScore;
        // for testing purposes going to return a 0 on all positions
        //return 0;
    }
};

