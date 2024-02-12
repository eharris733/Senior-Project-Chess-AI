#pragma once
#include "chess.hpp"
#include "evaluator.hpp"
#include "t_table.hpp"
#include <climits> // For INT_MIN and INT_MAX
#include <cstdlib>
#include <ctime>
#include <map>
#include <iostream>
#include <algorithm>
#include <chrono> // for timer

using namespace chess;

// Struct to hold the results of a search
struct SearchResult {
    Move bestMove;
    Move bestMove2; // last depth's best move
    Move killerMoves[2]; // store two killer moves
    int depth;
    int score;
    int nodes;
};

// variable that is called when the engine is told to stop searching
// will also be used for time control
extern std::atomic<bool> stop;

class Searcher {
public:
    Searcher(Board& initialBoard) : board(initialBoard), evaluator(initialBoard), tt(1 << 26){
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
        result.score = INT_MIN;
        result.nodes = 0;

        // clear the table for every search
        //tt.clear();

        // while we haven't been told to stop, and we haven't reached the desired think time
        while (result.depth < 7) {
            int bestScore = INT_MIN;
            int alpha = INT_MIN;
            int beta = INT_MAX;

            // // check to see if we have reached the desired think time
            // auto currentTime = chrono::steady_clock::now();
            // auto elapsed = chrono::duration_cast<chrono::milliseconds>(currentTime - startTime).count();

            // if (elapsed >= timeForThisMove) {
            //     stop = true; // Signal to stop the search
            //     break;
            // }

            Movelist moves;
            movegen::legalmoves<MoveGenType::ALL>(moves, board);
            sortMoves(moves, board, result); // we don't have a pv yet

            for (const Move& move : moves) {
                //if (stop.load()) break; // Check for stop signal

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
    int MAX_DEPTH = 100;

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

        // uint64_t zobristKey = board.zobrist();
        // auto ttEntry = tt.retrieve(zobristKey);

        // if (ttEntry.has_value() && ttEntry->depth >= depth) {
        //     if ((ttEntry->nodeType == NodeType::EXACT) ||
        //         (ttEntry->nodeType == NodeType::LOWERBOUND && ttEntry->score > alpha) ||
        //         (ttEntry->nodeType == NodeType::UPPERBOUND && ttEntry->score < beta)) {
        //         return ttEntry->score; // Use the score from the transposition table.
        //     }
        // }

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
        result.nodes ++;
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
                // store history and killer moves if move is quiet here
                    // update killer moves
                    if (move != result.killerMoves[0]) {
                        result.killerMoves[1] = result.killerMoves[0];
                        result.killerMoves[0] = move;
                    }

                break; // Alpha-beta cutoff.
            }
        }

        // Update the transposition table with the new best score and move found at this depth.
        //tt.save(zobristKey, depth, bestScore, determineNodeType(bestScore, alpha, beta), localBestMove);
        
        return bestScore;
    }


    // MVV-LVA (Most Valuable Victim - Least Valuable Aggressor) score
    // want to maybe change this to SEE eventually
    int MVV_LVA_Score(const Move& move, const Board& board, const SearchResult& result) {

        std::map<PieceType, int> PieceValue = {
        {PieceType::PAWN, 100},
        {PieceType::KNIGHT, 300},
        {PieceType::BISHOP, 325},
        {PieceType::ROOK, 500},
        {PieceType::QUEEN, 900},
        {PieceType::KING, 20000}
    };

        if (move == result.bestMove) return 100000; // PV move
        if (move == result.killerMoves[0] || move == result.killerMoves[1]) return 1; // Killer move

        // Assume Move has methods to get the positions and Board can give you the piece at a position
        PieceType victim = board.at<PieceType>(move.to());
        PieceType aggressor = board.at<PieceType>(move.from());


        if (victim == PieceType::NONE) { // not a capture
            return 0; // Quiet move
        }

        // MVV-LVA score is calculated as the value of the victim minus the value of the aggressor
        // We multiply the victim's value by a large number to ensure that captures are always evaluated first
        // and then subtract the aggressor's value to prioritize lower-value aggressors.
        return PieceValue[victim] * 10 - PieceValue[aggressor];
    }


    // sorts in order of 
    // PV, then captures, then everything else
    
// Sorting moves with corrected lambda capture
void sortMoves(Movelist& moves, const Board& board, const SearchResult& result) {
    std::sort(moves.begin(), moves.end(), [this, &board, &result](const Move& a, const Move& b) {
        return this->MVV_LVA_Score(a, board, result) > this->MVV_LVA_Score(b, board, result);
    });
}

    int evaluate(int depth) {
        int rawScore = evaluator.evaluate(depth); // Positive for White's advantage, negative for Black's
        return board.sideToMove() == Color::WHITE ? rawScore : -rawScore;
    }
};
