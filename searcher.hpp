#pragma once
#include "chess.hpp"
#include "evaluator.hpp"
#include "t_table.hpp"
#include "polyglot.hpp"
#include <cstdlib>
#include <ctime>
#include <map>
#include <iostream>
#include <algorithm>
#include <chrono> 
#include "tbprobe.h"





using namespace chess;
using namespace std;

// this version of iterative deepening is heavily influened 
// by Sebastian Lague's chess engine tutorial


// this is experimental as of right now
struct AspirationWindow {
    int alpha;
    int beta;
    int failHigh;
    int failLow;
};

// Struct to hold the state of a search
struct SearchState {
    Move bestMove;
    int bestScore;
    Move currentIterationBestMove;
    int currentIterationBestScore;
    Move killerMoves[2]; // store two killer moves
    int currentDepth;
    int nodes;
    AspirationWindow aspirationWindow;
    bool isOpening;
};

// for simple pruning techniques
static const std::map<PieceType, int> PieceValue = {
        {PieceType::NONE, 0},
        {PieceType::PAWN, 100},
        {PieceType::KNIGHT, 320},
        {PieceType::BISHOP, 330},
        {PieceType::ROOK, 500},
        {PieceType::QUEEN, 900},
        {PieceType::KING, 20000}
    };




// variable that is called when the engine is told to stop searching
// will also be used for time control
extern std::atomic<bool> stop;

class Searcher {
public:

    SearchState state;
    Searcher(Board& initialBoard) : board(initialBoard), evaluator(initialBoard), tt(1 << 22), book("openingbook/Cerebellum_Light_3Merge_200916/Cerebellum3Merge.bin"){
    }
    Move search(int timeRemaining, int timeIncrement, int movesToGo) {
        initSearch();
        // crude time control
        // we divide by 5 because the last depth is probably going to be 5 times as long as the rest

        // play an opening move if we can
        if(state.isOpening){
            Move openingMove = book.pickRandomMove(board);
            if (openingMove != Move::NULL_MOVE) {
                return openingMove;
            }
            state.isOpening = false; // if we make it to this line, the move is null
        }
       

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

            
            state.bestScore = negamax(state.currentDepth, neg_infinity, infinity, 0);
            

            // if we don't want to stop, keep going, otherwise, the search at that depth is thrown away
            // update the overall search state with results from the latest iteration
            if (!stop){
                
                // make sure we never return a null move
                if( state.currentIterationBestMove != Move::NULL_MOVE){
                    state.bestScore = state.currentIterationBestScore;
                    state.bestMove = state.currentIterationBestMove; // Update result only once,
                }

                 
                cout << " info depth " << state.currentDepth << " score cp " << state.bestScore << " pv " << uci::moveToUci(state.bestMove) << " nodes " << state.nodes << endl;

                 // stop if we have a mate, since it will never be a false mate
                 if(state.currentIterationBestScore > mateScore - MAX_DEPTH){
                     break;
                 }
                state.currentDepth++; // Update the depth after each iteration
                
            }
            //otherwise we abort the iterative deepening loop
            else{
                break;
            }
            
        }

        // debug function
        tt.debugSize();
        return state.bestMove; // Return the best move found
    }

    void clear() {
        tt.clear();
    }

private:
    Board& board; // The board to search on
    Evaluator evaluator; // our evaluation function
    TranspositionTable tt; // Transposition table
    PolyglotBook book; // Opening book
    int MAX_DEPTH = 100;
    
    // define my own versions of infinity and negative infinity (stolen again from Sebastian Lague's chess engine tutorial)
    const int infinity = 9999999;
    const int neg_infinity = -infinity;

    //mate score is less than the infinities in magnitude (IMPORTANT FOR NEGAMAX)
    const int mateScore = 100000;
    
    // probably unnecesary but good practice
    void initSearch() {
        state.isOpening = true;
        state.bestMove = Move::NULL_MOVE;
        state.bestScore = neg_infinity;
        state.currentDepth = 1;
        state.nodes = 0;
        state.currentIterationBestMove = Move::NULL_MOVE;
        state.currentIterationBestScore = neg_infinity;
        state.killerMoves[0] = Move::NULL_MOVE;
        state.killerMoves[1] = Move::NULL_MOVE;
        state.aspirationWindow = {neg_infinity, infinity, 0, 0};
    }

    int negamax(int depth, int alpha, int beta, int plyFromRoot) {
        // our search has been told to stop due to time
        // if (stop.load()){
        //     return 0;
        // }
        
        if (board.isRepetition() || board.isInsufficientMaterial() || board.isHalfMoveDraw()) {
            return 0; // Draw score
        }

       
        //pruning and hueristics booleans
        // important var for mate checks and search extensions
        bool isPvs = (beta - alpha) > 1;
        bool isCheck = board.inCheck();
        bool isRoot = plyFromRoot == 0;

       
        
    
        

        uint64_t zobristKey = board.zobrist();
        auto ttEntry = tt.retrieve(zobristKey);

        if (ttEntry.has_value() && ttEntry->depth >= depth) {
            if ((ttEntry->nodeType == NodeType::EXACT) ||
                (ttEntry->nodeType == NodeType::LOWERBOUND && ttEntry->score >= beta) ||
                (ttEntry->nodeType == NodeType::UPPERBOUND && ttEntry->score <= alpha)) {
                    if (isRoot) {
                        state.currentIterationBestMove = ttEntry->bestMove;
                        state.currentIterationBestScore = ttEntry->score;
                    }
                    return ttEntry->score; // Use the score from the transposition table.
                }
        }


            //null move reduction, 
            // based on https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=751ea2e28e427d6119be46de14be5140f7eb471e
            // might wantn to disable for pv search, or if we are doing lmr
            bool nullMoveReduction = depth >= 3 && !isCheck && !isRoot && !isPvs && evaluate(depth) >= beta;
            if (nullMoveReduction) {
                board.makeNullMove();
                int r = depth > 6 ? 4 : 3;
                int nullMoveScore = -negamax(depth - r, -beta, -beta + 1, plyFromRoot + 1);
                board.unmakeNullMove();
                if (nullMoveScore >= beta) {
                    // reduction in depth
                    depth -= 4;
                    if (depth <= 0) {
                        return evaluate(0);
                    }
                }
            }

            // var to keep track of how many moves we've looked at from the current node, 
            // used for late move reductions
            int moveCount = 0;

            Move localBestMove = Move::NULL_MOVE;
            NodeType nodeType = NodeType::UPPERBOUND;
            Movelist moves;
            movegen::legalmoves<MoveGenType::ALL>(moves, board);

            if (moves.size() == 0) {
                // Check for checkmate or stalemate
                return isCheck ? (-mateScore + plyFromRoot) : 0;
            }

            if (depth == 0) {
                return quiescence(alpha, beta); // Leaf node evaluation, add quiescence search here
            }
            state.nodes++;
            sortMoves(moves, board); // Pre-sort moves based on heuristics

            isPvs = true; // hash move will always be the first one searched

            for (const Move& move : moves) {

                //we should already know this from our sort moves
                bool isCapture = board.isCapture(move);
                bool isCheck = board.inCheck();

                // // Futility Pruning at depth 1
                // const int FUTILITY_MARGIN = 200; // Worth about two pawns
                // // Constraints: Not root node, not in check, not a capture, not a mate search, and depth is 1
                // if (plyFromRoot > 0 && !isCheck && !isCapture && !isPvs && depth == 1
                //     && !(abs(alpha) > mateScore - MAX_DEPTH || abs(beta) < mateScore + MAX_DEPTH)) { // Assuming abs() checks are what's intended
                //     float evaluation = evaluate(1); 
                //     if (evaluation + FUTILITY_MARGIN <= alpha) {
                //         // Consider returning alpha to indicate this branch doesn't improve upon the current best known score
                //         return alpha; 
                //     }
                // }

                board.makeMove(move);
                // recheck our pruning conditions
                isCheck = board.inCheck();
                int depthExtension = 0;
                //check extension in the right place now
                if (isCheck) {
                    depthExtension++;
                }

                // LMR condition
                // common conditions for LMR from chessprogramming wiki
                /*
                Most programs do not reduce these types of moves:

                Tactical Moves (captures and promotions)
                Moves while in check
                Moves which give check
                Moves that cause a search extension
                Anytime in a PV-Node in a PVS search
                Depth < 3 (sometimes depth < 2)*/

                //possible other conditions to consider
                // passed pawn moves (might even want to make them an extension

                // right now we only reduce by 1 move, but it is likely that with better move sorting
                // we can reduce by more the more moves we look at
                // also only going to look at depth 3 and on
                bool doLMR = depth >= 3 && moveCount > 3 && !isRoot && !isCapture && !isCheck && !isPvs && depthExtension == 0;
                // if (doLMR) {
                //     int reduction = max(1.0, log(depth) * log(moveCount) / 2);
                //     depth -= reduction; // Apply dynamic reduction based on depth and move count
                // }
                if (doLMR) {
                    depthExtension -= 1; // Apply dynamic reduction based on depth and move count
                }

                //Principal Variation Search
                int eval;
                // we have a pvs after one search
                if (isPvs) {
                    eval = -negamax(depth - 1 + depthExtension, -beta, -alpha, plyFromRoot + 1);
                }
                // zero window search, to try and prune a bunch
                else {
                    eval = -negamax(depth - 1 + depthExtension, -alpha - 1, -alpha, plyFromRoot + 1);

                    // move is better than expected, so we do a full research
                    if (eval > alpha && eval < beta) {
                        // The move is better than expected, re-search with a full window
                        eval = -negamax(depth - 1 + depthExtension, -beta, -alpha, plyFromRoot + 1);
                    }
                }
                board.unmakeMove(move);

                // it is now false
                isPvs = false;

                if (eval >= beta) {
                    tt.save(zobristKey, depth, beta, NodeType::LOWERBOUND, move);
                    // store history and killer moves if move is quiet here
                    // update killer moves only if not a capture

                    if (!isCapture && move != state.killerMoves[0]) {
                        state.killerMoves[1] = state.killerMoves[0];
                        state.killerMoves[0] = move;
                    }

                    return beta; // Alpha-beta cutoff.
                }

                if (eval > alpha) {
                    alpha = eval;
                    localBestMove = move; // Found a new best move.
                    nodeType = NodeType::EXACT;
                    if (isRoot) {
                        state.currentIterationBestMove = localBestMove;
                        state.currentIterationBestScore = alpha;
                    }
                }
                moveCount++;
            }

            // Update the transposition table with the new best score and move found at this depth.
            // right now only the exact node type works

            tt.save(zobristKey, depth, alpha, nodeType, localBestMove);



            return alpha;
        }


        //add delta pruning in the future
        int quiescence(int alpha, int beta) {
            // if(stop.load()){
            //     return 0;
            // }
            const int DELTA_MARGIN = 900; // most positional advantages aren't worth a rook
            state.nodes++;
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
                // Delta pruning
                if (stand_pat + DELTA_MARGIN + PieceValue.at(board.at<PieceType>(move.to())) <= alpha) {
                    alpha = alpha;
                }

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
            int MVV_LVA_Score(const Move& move, const Board& board) {
                if (move == state.bestMove) {
                    return infinity; // PV move
                }

                PieceType victim = board.at<PieceType>(move.to()); // piece being captured
                PieceType aggressor = board.at<PieceType>(move.from()); // piece capturing

                // Promotion check refined
                bool isPromotion = aggressor == PieceType::PAWN && (move.to() / 8 == 0 || move.to() / 8 == 7);

                if (victim == PieceType::NONE) {
                    if (isPromotion) {
                        return 200; // Base score for promotions
                    }
                    if (move == state.killerMoves[0] || move == state.killerMoves[1]) {
                        return 100; // Score for killer moves
                    }
                    return 10; // Base score for quiet moves
                }

                // MVV-LVA Scoring refined
                int score = PieceValue.at(victim) * 10 + PieceValue.at(PieceType::KING) - PieceValue.at(aggressor);
                return score;
            }

        // Sorting moves based on MVV-LVA score and PV and Killer moves
        void sortMoves(Movelist& moves, const Board& board) {
            std::sort(moves.begin(), moves.end(), [this, &board](const Move& a, const Move& b) {
                return this->MVV_LVA_Score(a, board) > this->MVV_LVA_Score(b, board);
            });
        }

        //trying out lazy evaluation
        int evaluate(int depth) {
            int rawScore = evaluator.evaluate(depth, false); // Positive for White's advantage, negative for Black's
            return board.sideToMove() == Color::WHITE ? rawScore : -rawScore;
        }

        //table base stuff, not test at all
        void initTablebases(std::string path) {
            tb_init(path.c_str());
        }

        #include "tbprobe.h"

        Move probeEndgame(const Board& board) {
            unsigned result = tb_probe_root(board.pieces, board.sideToMove, board.castlingRights, board.epSquare, board.fiftyMoveCounter, &info);
            if (result != TB_RESULT_FAILED) {
                unsigned move = TB_GET_FROM(result) | TB_GET_TO(result) | (TB_GET_PROMOTES(result) << 12);
                // Convert the move from Fathom format to your engine's move format
                return convertToFathomMove(move, board);
            }
            return Move::NO_MOVE; // Indicate failure or no tablebase entry
        }

    };
