#pragma once
#include "chess.hpp"
#include "evaluator.hpp"
#include "t_table.hpp"
#include <cstdlib>
#include <ctime>
#include <map>
#include <iostream>
#include <algorithm>
#include <chrono> 

using namespace chess;
using namespace std;

// this version of iterative deepening is heavily influened 
// by Sebastian Lague's chess engine tutorial

// Struct to hold the state of a search
struct SearchState {
    Move bestMove;
    int bestScore;
    Move currentIterationBestMove;
    int currentIterationBestScore;
    Move killerMoves[2]; // store two killer moves
    int currentDepth;
    int nodes;
};





// variable that is called when the engine is told to stop searching
// will also be used for time control
extern std::atomic<bool> stop;

class Searcher {
public:
    SearchState state;
    Searcher(Board& initialBoard) : board(initialBoard), evaluator(initialBoard), tt(1 << 20){
    }
    Move search(int timeRemaining, int timeIncrement, int movesToGo) {
        initSearch();
        // crude time control
        // we divide by 2 because the last depth is probably going to be more than 5 times as long as the rest
        int timeForThisMove = (timeRemaining / movesToGo + timeIncrement) / 5;

        // // start the timer
        auto startTime = chrono::steady_clock::now();

        // while we haven't been told to stop, and we haven't reached the desired think time
        while (state.currentDepth < MAX_DEPTH && !stop.load()) {
            state.currentIterationBestMove = Move::NULL_MOVE;
            state.currentIterationBestScore = neg_infinity;
            
            // check to see if we have reached the desired think time
            auto currentTime = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(currentTime - startTime).count();

            if (elapsed >= timeForThisMove) {
                stop = true; // Signal to stop the search
                break;
            }
            // call the search function
            negamax(state.currentDepth, neg_infinity, infinity, true); // isroot = true

            // if we don't want to stop, keep going, otherwise, the search at that depth is thrown away
            // update the overall search state with results from the latest iteration
            if (!stop){
                state.bestScore = state.currentIterationBestScore;
                state.bestMove = state.currentIterationBestMove; // Update result only once,
                cout << " info depth " << state.currentDepth << " score cp " << state.bestScore << " pv " << uci::moveToUci(state.bestMove) << " nodes " << state.nodes << endl;
                state.currentDepth++; // Update the depth after each iteration
                
            }
            //otherwise we abort the iterative deepening loop
            else{
                break;
            }
            
        }

        // debug function
        tt.debugSize();
        return state.bestMove; // Return the search result
    }

    void clear() {
        tt.clear();
    }

private:
    Board& board; // The board to search on
    Evaluator evaluator; // our evaluation function
    TranspositionTable tt; // Transposition table
    int MAX_DEPTH = 100;
    
    // define my own versions of infinity and negative infinity (stolen again from Sebastian Lague's chess engine tutorial)
    const int infinity = 9999999;
    const int neg_infinity = -infinity;

    //mate score is less than the infinities in magnitude (IMPORTANT FOR NEGAMAX)
    const int mateScore = 100000;
    
    // probably unnecesary but good practice
    void initSearch() {
        state.bestMove = Move::NULL_MOVE;
        state.bestScore = neg_infinity;
        state.currentDepth = 1;
        state.nodes = 0;
        state.currentIterationBestMove = Move::NULL_MOVE;
        state.currentIterationBestScore = neg_infinity;
        state.killerMoves[0] = Move::NULL_MOVE;
        state.killerMoves[1] = Move::NULL_MOVE;
    }

    int negamax(int depth, int alpha, int beta, bool isRoot = false) {
        // our search has been told to stop due to time
        if (stop.load()){
            return 0;
        }
        
        if (board.isRepetition() || board.isInsufficientMaterial() || board.isHalfMoveDraw()) {
            return 0; // Draw score
        }

        uint64_t zobristKey = board.zobrist();
        auto ttEntry = tt.retrieve(zobristKey);

        if (ttEntry.has_value() && ttEntry->depth >= depth) {
            if ((ttEntry->nodeType == NodeType::EXACT) ||
                (ttEntry->nodeType == NodeType::LOWERBOUND && ttEntry->score > alpha) ||
                (ttEntry->nodeType == NodeType::UPPERBOUND && ttEntry->score < beta)) {
                if (isRoot){
                    state.currentIterationBestMove = ttEntry->bestMove;
                    state.currentIterationBestScore = ttEntry->score;
                }
                return ttEntry->score; // Use the score from the transposition table.
            }
        }
        Move localBestMove = Move::NULL_MOVE;
        NodeType nodeType = NodeType::UPPERBOUND;
        Movelist moves;
        movegen::legalmoves<MoveGenType::ALL>(moves, board);

        if (moves.size() == 0) {
            // Check for checkmate or stalemate
            return board.inCheck() ? (-mateScore - depth) : 0;
        }

        if (depth == 0) {
            return quiescence(alpha, beta); // Leaf node evaluation, add quiescence search here
        }
        state.nodes ++;
        sortMoves(moves, board); // Pre-sort moves based on heuristics

        for (const Move& move : moves) {
            board.makeMove(move);
            int eval = -negamax(depth - 1, -beta, -alpha, false); 
            board.unmakeMove(move);
            
            if (eval >= beta) {
                //tt.save(zobristKey, depth, beta, NodeType::LOWERBOUND, move); 
                // store history and killer moves if move is quiet here
                    // update killer moves
                    if (move != state.killerMoves[0]) {
                        state.killerMoves[1] = state.killerMoves[0];
                        state.killerMoves[0] = move;
                    }

                return beta; // Alpha-beta cutoff.
            }

            if (eval > alpha) {
                alpha = eval;
                localBestMove = move; // Found a new best move.
                nodeType = NodeType::EXACT;
                if (isRoot){
                    state.currentIterationBestMove = localBestMove;
                    state.currentIterationBestScore = alpha;
                }
            }
            
        }

        // Update the transposition table with the new best score and move found at this depth.
       tt.save(zobristKey, depth, alpha, nodeType, localBestMove);
        
        return alpha;
    }

    int quiescence(int alpha, int beta) {
        int stand_pat = evaluate(0);
        if (stand_pat >= beta) {
            return beta;
        }
        if (alpha < stand_pat) {
            alpha = stand_pat;
        }
        Movelist moves;
        movegen::legalmoves<MoveGenType::CAPTURE>(moves, board);
        sortMoves(moves, board);
        for (const Move& move : moves) {
            board.makeMove(move);
            int score = -quiescence(-beta, -alpha);
            board.unmakeMove(move);
            if (score >= beta) {
                return beta;
            }
            if (score > alpha) {
                alpha = score;
            }
        }
        return alpha;
    }

constexpr int rank_of(Square sq) {
    return static_cast<int>(sq) / 8; // Divide the square index by 8 to get the rank
}
        // Improved MVV-LVA scoring function
int MVV_LVA_Score(const Move& move, const Board& board) {
    if (move == state.bestMove) {
        return infinity; // PV move
    }   

    static const std::map<PieceType, int> PieceValue = {
        {PieceType::NONE, 0},
        {PieceType::PAWN, 100},
        {PieceType::KNIGHT, 320},
        {PieceType::BISHOP, 330},
        {PieceType::ROOK, 500},
        {PieceType::QUEEN, 900},
        {PieceType::KING, 20000}
    };
    // Correctly determine the victim and aggressor pieces

    PieceType victim = board.at<PieceType>(move.to()); // Victim is the piece on the destination square
    PieceType aggressor = board.at<PieceType>(move.from()); // Aggressor is the piece on the source square

    // If there is no capture, return a low base score for quiet moves
    if (victim == PieceType::NONE) {
        // check to see if it is a promotion
        if (aggressor == PieceType::PAWN && (static_cast<int>(move.to()) / 8 == 0) == 0 || (static_cast<int>(move.to()) / 8 == 7) == 0){
            return 200; // Base score for promotions
        }
        if (move == state.killerMoves[0] || move == state.killerMoves[1]){
            return 100; // Better than base score for killer moves
        }   
        return 10; // Base score for quiet moves to differentiate them from invalid moves
    }

    // Calculate and return the MVV-LVA score
    // Prioritize captures by victim value, and within those, prioritize lower-value aggressors
    int score = PieceValue.at(victim) * 10 + PieceValue.at(PieceType::KING) - PieceValue.at(aggressor);
    assert(score > 0); // Ensure scoring logic is correct and positive scores are assigned to valid moves
    return score;
}


    
// Sorting moves based on MVV-LVA score and PV and Killer moves
void sortMoves(Movelist& moves, const Board& board) {
    std::sort(moves.begin(), moves.end(), [this, &board](const Move& a, const Move& b) {
        return this->MVV_LVA_Score(a, board) > this->MVV_LVA_Score(b, board);
    });
}

    int evaluate(int depth) {
        int rawScore = evaluator.evaluate(depth); // Positive for White's advantage, negative for Black's
        return board.sideToMove() == Color::WHITE ? rawScore : -rawScore;
    }
};
