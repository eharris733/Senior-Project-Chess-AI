#pragma once
#include "chess.hpp"
#include "evaluator.hpp"
#include "t_table.hpp"
#include "polyglot.hpp"
#include "baselines.hpp"   
#include "ga_util.hpp" 
#include "ga1results.hpp"
#include <cstdlib>
#include <ctime>
#include <map>
#include <iostream>
#include <algorithm>
#include <chrono> 
#include <cstring>
#include <atomic>
#include <memory>

using namespace chess;
using namespace std;

//TODO: Fractional extensions, where most moves are extended by a fraction of a ply if certain conditions are met, as well as LMR can be fractional too. Can add these paramters to the search parameters struct, 
// to allow for tuning

// this is experimental as of right now
struct AspirationWindow {
    int alpha = 0;
    int beta = 0;
    int delta = 0;
    int failHigh = 0;
    int failLow = 0;
};

// Struct to hold the state of a search
struct SearchState {
    Move bestMove = Move::NULL_MOVE;
    int bestScore = 0;
    Move currentIterationBestMove = Move::NULL_MOVE;
    int currentIterationBestScore = 0;
    Move killerMoves[2]; // store two killer moves
    int currentDepth = 1;
    long nodes = 0;
    AspirationWindow aspirationWindow = AspirationWindow();
    bool isOpening = true;
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



class Searcher {
public:

    
    Searcher(Board& initialBoard, TunableSearch searchParams = baseSearch, TunableEval evalParams = ga1result10) 
        : board(initialBoard), 
          evaluator(initialBoard, evalParams), 
          tt(1 << 22), // this value is arbitrary, but it should be a power of 2, setting it to rly small for time
          //book("openingbook/Cerebellum_Light_3Merge_200916/Cerebellum3Merge.bin"),
          searchParams(searchParams)
          {
        state.bestScore = 0; // only at the beginning of the game do we assume an eval of 0
        state.nodes = 0;
        state.aspirationWindow = AspirationWindow();
        std::memset(history, 0, sizeof(history)); 

    }

    // getter method for the state of the search
    SearchState getState() {
        return state;
    }

    // setter method for the search parameters, so we can set to 1 or 2 or 6 for training
    void setMaxDepth(int depth) {
        MAX_DEPTH = depth;
    }   

    void setVerbose(bool v) {
        verbose = v;
    }

    SearchState search(int timeRemaining, int timeIncrement, int movesToGo) {
        initSearch();
        int eval = 0;
        // disabling this for now while I'm training the GA
        // // play an opening move if we can, this adds one lookup at the beginning of the search, so not 
        // // perfect, but not really a factor of performance
        // Move openingMove = book.pickRandomMove(board);
        // if (openingMove != Move::NULL_MOVE) {
        //     state.bestMove = openingMove;
        // }
       
        // add an offset to our timeForThisMove if we are just out of the opening possible?
        // we subtract 10 for overhead
        timeForThisMove = (timeRemaining / movesToGo + timeIncrement) - 10;

        // // start the timer
        start_t = std::chrono::high_resolution_clock::now();

        // while we haven't been told to stop, and we haven't reached the desired think time
        while (state.currentDepth <= MAX_DEPTH && !stopSearching) {
            state.currentIterationBestMove = Move::NULL_MOVE;
            state.currentIterationBestScore = neg_infinity;

            if(state.currentDepth < searchParams.useAspirationWindowDepth){

                eval = negamax(state.currentDepth, neg_infinity, infinity, 0);
                if (!stopSearching){
                    state.bestScore = eval;
                }
                else{
                    break;
                
                }
            }
            // at depth x we use aspiration windows
            else{
                int eval = aspirationSearch();
                // only update if we haven't been told to stop
                if (!stopSearching){
                    state.bestScore = eval;
                }
                else{
                    break;
                }
                
            }
            
            //state.bestScore = negamax(state.currentDepth, neg_infinity, infinity, 0);

            // if we don't want to stop, keep going, otherwise, the search at that depth is thrown away
            // update the overall search state with results from the latest iteration
                
                // make sure we never return a null move
                if( state.currentIterationBestMove != Move::NULL_MOVE && !stopSearching){
                    state.bestScore = state.currentIterationBestScore;
                    state.bestMove = state.currentIterationBestMove; // Update result only once,
                }

                if (verbose){
                    cout << " info depth " << state.currentDepth << " score cp " << state.bestScore << " pv " << uci::moveToUci(state.bestMove) << " nodes " << state.nodes << endl;
                }
                

                 // stop if we have a mate, perhapds be a little more catius with this?
                 if(state.currentIterationBestScore > mateScore - MAX_DEPTH){
                     break;
                 }
                state.currentDepth++; // Update the depth after each iteration
                
            
        }

        // debug function
        if(verbose){
            tt.debugSize();
        }
        if (state.bestMove == Move::NULL_MOVE) {
            Movelist moves;
            movegen::legalmoves<MoveGenType::ALL>(moves, board);
            state.bestMove = moves[0]; // If no move is found, return the first legal move
        }
        return state; // Return the best move found
    }

    void clear() {
        tt.clear();
        std::memset(history, 0, sizeof(history)); // set everything back to 0
    }

private:
    Board& board; // The board to search on
    Evaluator evaluator = Evaluator(board, evalParams); // our evaluation function
    SearchState state = SearchState();
    TranspositionTable tt; // Transposition table
    //PolyglotBook book; // Opening book (commenting out for traiing purposes)
    TunableSearch searchParams; // Search parameters  
    TunableEval evalParams; // Evaluation parameters
    int history[2][6][64]; // history heuristic table
    int MAX_DEPTH = 100;
    bool verbose = true;
    std::chrono::system_clock::time_point start_t;  // search start time
    int timeForThisMove = 0;
    bool stopSearching = false;
    
    // define my own versions of infinity and negative infinity (stolen again from Sebastian Lague's chess engine tutorial)
    const int infinity = 9999999;
    const int neg_infinity = -infinity;

    //mate score is less than the infinities in magnitude (IMPORTANT FOR NEGAMAX)
    const int mateScore = 100000;
    
    // probably unnecesary but good practice
    void initSearch() {
        stopSearching = false;
        state.isOpening = true;
        state.bestMove = Move::NULL_MOVE;
        state.currentDepth = 1;
        state.nodes = 0;
        state.currentIterationBestMove = Move::NULL_MOVE;
        state.currentIterationBestScore = neg_infinity;
        state.killerMoves[0] = Move::NULL_MOVE; // maybe don't reset them?
        state.killerMoves[1] = Move::NULL_MOVE;
        std::memset(history, 0, sizeof(history)); // set everything back to 0
        start_t = std::chrono::system_clock::now();
    }

    bool isTimeOver() {
        // otherwise, check timeover every 2048 nodes
        if (!(state.nodes & 2047)) {
            auto now = std::chrono::high_resolution_clock::now();
            auto dtime = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_t).count();
            if (dtime >= timeForThisMove){
                stopSearching = true;
            }
        }
        return stopSearching;
    }


    // Aspiration window search
int aspirationSearch() {
    // 
    bool isOpenWindow = false;

    int eval;
    // Reset or initialize aspiration window parameters
    state.aspirationWindow.delta = 20;
    state.aspirationWindow.alpha = state.bestScore - state.aspirationWindow.delta;
    state.aspirationWindow.beta = state.bestScore + state.aspirationWindow.delta;
    state.aspirationWindow.failLow = 0;
    state.aspirationWindow.failHigh = 0;

    // Ensure initial values for alpha, beta, and delta are set before entering this function.

    while (!isOpenWindow){
        eval = negamax(state.currentDepth, state.aspirationWindow.alpha, state.aspirationWindow.beta, 0);
        
        // if we found a mate score, we can stop
        // deleting this since we are not neccesarily searching all the moves, so we can def find a false mate
        // if(eval > mateScore - MAX_DEPTH || eval < -mateScore + MAX_DEPTH){
        //     return eval;
        // }
        // Update our aspiration windows based on the evaluation
        if (eval <= state.aspirationWindow.alpha) {
            // Fail low, widen the window downwards
            
            state.aspirationWindow.beta = state.aspirationWindow.alpha; // Adjust beta to the current alpha
            
            if(state.aspirationWindow.failLow < 2){
                if (state.aspirationWindow.failLow == 0){
                    state.aspirationWindow.delta += searchParams.aspirationWindow1;
                }
                else{
                    state.aspirationWindow.delta += searchParams.aspirationWindow2;
                }
                
                state.aspirationWindow.alpha -= state.aspirationWindow.delta; // Decrease alpha
            }
            else{
                isOpenWindow = true;
            }
            
        state.aspirationWindow.failLow++;
            
        } else if (eval >= state.aspirationWindow.beta) {
            // Fail high, widen the window upwards
            
            state.aspirationWindow.alpha = state.aspirationWindow.beta; // Adjust alpha to the current beta
            
            if(state.aspirationWindow.failHigh < 2){
                if (state.aspirationWindow.failHigh == 0){
                    state.aspirationWindow.delta += searchParams.aspirationWindow1; 
                }
                else{
                    state.aspirationWindow.delta += searchParams.aspirationWindow2;
                }
                
                state.aspirationWindow.beta += state.aspirationWindow.delta; // Increase beta
            }
            else{
                isOpenWindow = true;
            }
            state.aspirationWindow.failHigh++;
        } else {
            // Score is within the window, search is successful
            return eval;
        }

        // Optional: Add a mechanism to prevent the loop from running indefinitely
        // For example, a maximum number of iterations or a condition to widen the window to a maximum allowed range
    }
    return negamax(state.currentDepth, neg_infinity, infinity, 0);
}



    int negamax(int depth, int alpha, int beta, int plyFromRoot) {
        if (isTimeOver() || stopSearching){
            return 0; // Stop signal received, return the worst possible score so results are ignored from this search
        }
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
            bool nullMoveReduction = depth >= 3 && !isCheck && !isRoot && !isPvs;
            if (nullMoveReduction) {
                int staticEval = evaluate(depth, searchParams.useLazyEvalNMR); // lazy eval to avoid expensive evals
                if (staticEval >= beta){
                    board.makeNullMove();
                    int r = depth > 6 ? 4 : 3;
                    int nullMoveScore = -negamax(depth - r, -beta, -beta + 1, plyFromRoot + 1);
                    board.unmakeNullMove();
                    if (nullMoveScore >= beta) {
                        // reduction in depth
                        depth -= 4;
                        if (depth <= 0) {
                            return staticEval;
                        }
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
            
            sortMoves(moves, board); // Pre-sort moves based on heuristics

            isPvs = true; // hash move will always be the first one searched

            for (const Move& move : moves) {

                //we should already know this from our sort moves
                bool isCapture = board.isCapture(move);
                bool isCheck = board.inCheck();
                int depthExtension = 0;

                // Reverse Futility Pruning at depth 1-3, with increasing margins
                //completely winning position
                // Constraints: Not root node, not in check, not a capture, not a mate search, and depth is 1
                // also can't prune only legal move
                if (!isRoot && !isCheck && !isPvs && !isCapture && moves.size() > 1 && depth <= 3
                    && !(abs(alpha) > mateScore - MAX_DEPTH || abs(beta) < mateScore + MAX_DEPTH)) { // Assuming abs() checks are what's intended
                    float evaluation = evaluate(depth, searchParams.useLazyEvalFutility); // lazy eval to avoid expensive evals
                    if (depth == 1 && evaluation + searchParams.futilityMargin1 <= alpha) {
                        // Consider returning alpha to indicate this branch doesn't improve upon the current best known score
                        continue;
                    }
                    else if (depth == 2 && evaluation + searchParams.futilityMargin2 <= alpha){
                        continue;
                    }
                    else if(depth == 3 && evaluation + searchParams.futilityMargin3 <= alpha){
                        continue;
                    }
                    
                }

                board.makeMove(move);
                state.nodes++;
                // recheck our pruning conditions
                isCheck = board.inCheck();
                PieceType pieceType = board.at<PieceType>(move.to());
                
                //check extension in the right place now
                if (isCheck) {
                    depthExtension = 1;
                }

                // extend if a passed pawn reaches the seventh rank
                // only if in pvs
                if(isPvs && board.at<PieceType>(move.to()) == PieceType::PAWN) {
                    if(board.sideToMove() == Color::WHITE && rank_of(move.to()) == 6){
                        depthExtension = 1;
                    }
                    else if(board.sideToMove() == Color::BLACK && rank_of(move.to()) == 1){
                        depthExtension = 1;
                    }
                }

                //note that any extension will only set the depth extension to 1, so we can't have multiple extensions

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
                bool doLMR = depth >= searchParams.initalDepthLMR && moveCount > searchParams.initialMoveCountLMR && !isRoot && !isCapture && !isCheck && !isPvs && depthExtension == 0;
                // if (doLMR) {
                //     int reduction = max(1.0, log(depth) * log(moveCount) / 2);
                //     depth -= reduction; // Apply dynamic reduction based on depth and move count
                // }
                if (doLMR) {
                    // another layer of reduction later in the search and late in the move order
                    // ideally would be percentage based
                    if(moveCount > searchParams.secondaryMoveCountLMR && depth >= searchParams.secondaryDepthLMR){
                        depthExtension -= 1; // Apply dynamic reduction based on depth and move count
                    }
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

                // make sure we don't update anything if we are told to stop
                if (stopSearching){
                    return 0;
                }

                if (eval >= beta) {
                    tt.save(zobristKey, depth, beta, NodeType::LOWERBOUND, move);
                    // store history and killer moves if move is quiet here
                    // update killer moves only if not a capture

                    if (!isCapture)  {
                        if (move != state.killerMoves[0]){
                            state.killerMoves[1] = state.killerMoves[0];
                            state.killerMoves[0] = move;
                        }
                        // update history
                        history[board.sideToMove() == Color::WHITE? 1: 0][static_cast<int>(board.at<PieceType>(move.from()))][static_cast<int>(move.to())] += depth * depth;
                        
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


        int quiescence(int alpha, int beta) {

            if (isTimeOver() || stopSearching){
                return 0; // Stop signal received, return the worst possible score so results are ignored from this search
            }

            const int DELTA_MARGIN = searchParams.deltaMargin; // most positional advantages aren't worth more than a piece
            
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

                // Delta pruning, disabled in the late endgame due to insufficient material considerations
                // also disabled for capture checks, since that probably is a good move
                if (evaluator.getGamePhase() > .1 && !board.inCheck() && stand_pat + DELTA_MARGIN + PieceValue.at(board.at<PieceType>(move.to())) <= alpha) {
                    continue; // prune this move
                }

                board.makeMove(move);
                state.nodes++; // ive changed to only updating node counts after we change board state
                int score = -quiescence(-beta, -alpha);
                board.unmakeMove(move);

                if (stopSearching){
                    return 0;
                }
                if (score >= beta) {
                    return beta;
                }
                if (score > alpha) {
                    alpha = score;
                }
            }
            return alpha;
        }
            // these utility functions could go in their own file tbh
            constexpr int rank_of(Square sq) {
                return static_cast<int>(sq) / 8; // Divide the square index by 8 to get the rank
            }

            // move ordering needs to be tuned
            int MVV_LVA_Score(const Move& move, const Board& board) {
                if (move == Move::NULL_MOVE) {
                    return 0;
                }
                if (move == state.bestMove) {
                    return infinity; // PV move
                }

                PieceType victim = board.at<PieceType>(move.to()); // piece being captured
                PieceType aggressor = board.at<PieceType>(move.from()); // piece capturing

                // Promotion check refined
                bool isPromotion = aggressor == PieceType::PAWN && (move.to() / 8 == 0 || move.to() / 8 == 7);

                if (victim == PieceType::NONE) {
                    if (isPromotion) {
                        return searchParams.promotionMoveScore; // Base score for promotions
                    }
                    if (move == state.killerMoves[0] || move == state.killerMoves[1]) {
                        return searchParams.killerMoveScore; // Score for killer moves
                    }
                    // should always be less than a killer move score
                    return std::min(history[board.sideToMove() == Color::WHITE? 1: 0][static_cast<int>(board.at<PieceType>(move.from()))][static_cast<int>(move.to())], searchParams.killerMoveScore - 1);
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
        int evaluate(int depth, bool isLazy = false) {
            int rawScore = evaluator.evaluate(depth, isLazy); // Positive for White's advantage, negative for Black's
            return board.sideToMove() == Color::WHITE ? rawScore : -rawScore;
        }

    };