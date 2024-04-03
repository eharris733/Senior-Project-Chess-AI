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
#include <chrono>

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
    int numMovesOutofBook = 0;
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
          //book("Titans.bin"), // this one has been the most reliable for me
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
        // play a random opening move from our book if we can
        // Move openingMove = book.pickRandomMove(board);
        // if (openingMove != Move::NULL_MOVE) {
        //     state.bestMove = openingMove;
        //     return state;
        // }
       
        // add an offset to our timeForThisMove if we are just out of the opening possible?
        // we subtract 50 for overhead
        // from chess programming wiki

        int timeRemainingWithOverhead = timeRemaining - 55;
        int nMoves = min(state.numMovesOutofBook, 10);
        int factor = 2 - nMoves / 10;
        int target = timeRemainingWithOverhead / movesToGo;
        timeForThisMove = factor * target; // offset by 50 just in case
        
        if (timeForThisMove < 50) {
            timeForThisMove =  50; // minimum time
        }

        // start the timer
        start_t = std::chrono::high_resolution_clock::now();

        // while we haven't been told to stop, and we haven't reached the desired think time
        while (state.currentDepth <= MAX_DEPTH && !stopSearching) {
            if (stopOnThisDepth()){
                break;
            }
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
                eval = aspirationSearch();
                // only update if we haven't been told to stop
                if (!stopSearching){
                    state.bestScore = eval;
                }
                else{
                    break;
                }
                
            }

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
        state.numMovesOutofBook ++;
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
    std::chrono::high_resolution_clock::time_point start_t;  // search start time
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
        start_t = std::chrono::high_resolution_clock::now();
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

    bool stopOnThisDepth() {
        auto now = std::chrono::high_resolution_clock::now();
        auto dtime = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_t).count();
        auto timeLeft = timeForThisMove - dtime;
        if (timeLeft * 4 <= timeForThisMove){ // essnetially we have no hope of finishing in time, and the results will be thrown away, so better to save time for later searches
            return true;
        }
        return false;
    }


    // Aspiration window search
    int aspirationSearch() {
        bool isOpenWindow = false;

        int eval = 0;
        // Reset or initialize aspiration window parameters
        state.aspirationWindow.delta = searchParams.aspirationWindowInitialDelta;
        state.aspirationWindow.alpha = state.bestScore - state.aspirationWindow.delta;
        state.aspirationWindow.beta = state.bestScore + state.aspirationWindow.delta;
        state.aspirationWindow.failLow = 0;
        state.aspirationWindow.failHigh = 0;

        // Ensure initial values for alpha, beta, and delta are set before entering this function.

        while (!isOpenWindow){
            eval = negamax(state.currentDepth, state.aspirationWindow.alpha, state.aspirationWindow.beta, 0);
            
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
        }
        return negamax(state.currentDepth, neg_infinity, infinity, 0);
    }


    int negamax(int depth, int alpha, int beta, int plyFromRoot) {

        if (depth <= 0) {
                return quiescence(alpha, beta); // Leaf node evaluation
            }

        if (isTimeOver() || stopSearching){
            return 0; // search will be thrown away
        }
        if (board.isRepetition() || board.isInsufficientMaterial() || board.isHalfMoveDraw()) {
            return 0; // Draw score (can add contempt later)
        }

       
        //pruning and hueristics booleans
        // important var for mate checks and search extensions
        bool isPvs = (beta - alpha) > 1;
        bool isCheck = board.inCheck();
        bool isRoot = plyFromRoot == 0;
        int depthExtension = 0;

        // check extension
        if (isCheck) {
            depthExtension = 1; // Extend search depth if in check
        }

        uint64_t zobristKey = board.zobrist();
        auto ttEntry = tt.retrieve(zobristKey);
        Move ttMove = Move::NULL_MOVE;
        int ttScore = 0;


        if (ttEntry.has_value()) {
            ttMove = ttEntry->bestMove;
            ttScore = ttEntry->score;
        } // store the tt move if it exists

        if (ttEntry.has_value() && ttEntry->depth >= depth) {
            if ((ttEntry->nodeType == NodeType::EXACT) ||
                (ttEntry->nodeType == NodeType::LOWERBOUND && ttEntry->score >= beta) ||
                (ttEntry->nodeType == NodeType::UPPERBOUND && ttEntry->score <= alpha)) {
                    if (isRoot) {
                        state.currentIterationBestMove = ttEntry->bestMove;
                        state.currentIterationBestScore = ttEntry->score;
                    }
                    else{
                        return ttEntry->score; // Use the score from the transposition table.
                    }
                    
                }
            }

            
            //null move pruning and reverse futility pruniing
            bool canPrune = !isCheck && !isRoot && !isPvs;
            if (canPrune) {
                int staticEval = ttMove != Move::NO_MOVE? ttScore: evaluate(depth); // lazy eval to avoid expensive evals
                // reverse futility pruning
                if (depth <= 3 &&
                    (abs(alpha) < (mateScore - MAX_DEPTH)) && (abs(beta) < (mateScore + MAX_DEPTH))) {
                    if (depth == 1 && (staticEval - searchParams.futilityMargin1 >= beta)) {
                        return staticEval - searchParams.futilityMargin1;
                    }
                    else if (depth == 2 && (staticEval - searchParams.futilityMargin2 >= beta)){
                        return staticEval - searchParams.futilityMargin2;
                    }
                    else if(depth == 3 && (staticEval - searchParams.futilityMargin3 >= beta)){
                        return staticEval - searchParams.futilityMargin3;
                    }
                }

                // null move pruning
                if (depth >= 3 && evaluator.getGamePhase() > .1 && staticEval >= beta){ 
                    board.makeNullMove();
                    int r = 3; // reduction amount (can probably be tuned, but currently going to leave it at this
                    int nullMoveScore = -negamax(depth - r, -beta, -beta + 1, plyFromRoot + 1);
                    board.unmakeNullMove();
                    if (nullMoveScore >= beta) {
                        return beta; // Null move cutoff
                    }
                }
                
            }

            // probcut and razoring would go here theoretically

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

            sortMoves(moves, board, ttMove); // Pre-sort moves based on heuristics

            for (const Move& move : moves) {

                //we should already know this from our sort moves
                bool isCapture = board.isCapture(move);
                bool isCheck = board.inCheck();
                

                board.makeMove(move);
                moveCount++;
                state.nodes++;
                // recheck our pruning conditions
                isCheck = board.inCheck();
                    
                //note that any extension will only set the depth extension to 1, so we can't have multiple extensions

                // LMR conditions
                
                    bool doLMR = depth >= searchParams.initalDepthLMR && 
                moveCount > searchParams.initialMoveCountLMR && 
                !isRoot  && !isCheck  && 
                depthExtension == 0;

                int eval = 0;
                bool doReSearch = false;

                if (doLMR) {
                    int  LMR_REDUCTION = 2; // Reduce depth by 1 for late move reductions
                    LMR_REDUCTION -= isPvs; // Adjust -1 or 0
                    LMR_REDUCTION -= isCapture; // Adjust -1 or 0
                    LMR_REDUCTION += (log(moveCount) + log(depth)) / 2.0; // logarithmic reduction, 1.75 stolen from small brain
                    int reducedDepth = depth - LMR_REDUCTION; // Adjust -1 or in
                    eval = -negamax(reducedDepth, -alpha - 1, -alpha, plyFromRoot + 1);
                    // Check if the reduced-depth search beats alpha, signaling a re-search is needed
                    doReSearch = eval > alpha;
                } else {
                    // No LMR; indicate a full-depth search is required
                    doReSearch = true;
                }

                // Re-search with full depth if the move seems promising or if LMR wasn't applied
                if (doReSearch) {
                    // If it was an LMR move that triggered the re-search, search with the original depth
                    // Otherwise, this block is executed for non-LMR moves or moves that naturally need a full-depth search
                    eval = -negamax(depth - 1, -alpha - 1, -alpha, plyFromRoot + 1);
                    
                    // After re-search, if the move is still promising, consider a full-window search for PV nodes
                    if (eval > alpha && eval < beta){
                        eval = -negamax(depth - 1, -beta, -alpha, plyFromRoot + 1);
                    }
                }

                board.unmakeMove(move);

                // it is now false, since we played the principal variation move
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
            }

            // Update the transposition table with the new best score and move found at this depth.

            tt.save(zobristKey, depth, alpha, nodeType, localBestMove);

            return alpha;
        }

        // Quiescence search
        int quiescence(int alpha, int beta) {

            if (isTimeOver() || stopSearching){
                return 0; // Stop signal received, move will be thrown away
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
                state.nodes++; // Ive changed to only updating node counts after we change board state
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

            // move ordering needs to be tuned
            int MVV_LVA_Score(const Move& move, const Board& board, Move ttMove = Move::NULL_MOVE) {
                if (move == Move::NULL_MOVE) {
                    std::cout << "NULL MOVE IN MVV_LVA" << std::endl;
                    return 0;
                }
                if (move == state.bestMove ) {
                    return infinity; // PV move
                }
                if (move == ttMove) {
                    return infinity - 1;
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
        void sortMoves(Movelist& moves, const Board& board, Move ttMove = Move::NULL_MOVE) {
            std::sort(moves.begin(), moves.end(), [this, &board, &ttMove](const Move& a, const Move& b) {
                return this->MVV_LVA_Score(a, board, ttMove) > this->MVV_LVA_Score(b, board, ttMove);
            });
        }

        //lazy evaluation just means we only evaluate material score
        int evaluate(int depth, bool isLazy = false) {
            int rawScore = evaluator.evaluate(depth, isLazy); // Positive for White's advantage, negative for Black's
            return board.sideToMove() == Color::WHITE ? rawScore : -rawScore;
        }
};