#include "chess.hpp"
#include "baselines.hpp"
#include "evaluator.hpp"
#include "t_table.hpp"

#include <chrono>
#include <map>

using namespace chess;
using namespace std;

struct AspirationWindow {
    int alpha = 0;
    int beta = 0;
    int delta = 0;
    int failHigh = 0;
    int failLow = 0;
};

const int MAXDEPTH = 100;

// Struct to hold the searchState of a search
struct SearchState {
    Move bestMove = Move::NULL_MOVE;
    int bestScore = 0;
    Move currentIterationBestMove = Move::NULL_MOVE;
    int currentIterationBestScore = 0;
    Move killerMoves[2][MAXDEPTH + 1]{}; // store two killer moves
    int currentDepth = 1;
    long nodes = 0;
    AspirationWindow aspirationWindow = AspirationWindow();
    // bool isOpening = true;
    int numMovesOutofBook = 0; 
};



class Searcher2 {
    public:
    Searcher2(Board& initialBoard, TunableSearch& searchParams = baseSearch, TunableEval& evalParams = baseEval)
    : board(initialBoard), searchParams(searchParams), evalParams(evalParams), evaluator(board, evalParams), tt(2 << 22){
        searchState = SearchState();
    }

    void initSearchState(){
        stopSearching = false;

        //searchState.isOpening = true;
        searchState.bestMove = Move::NULL_MOVE;
        searchState.currentDepth = 1;
        searchState.nodes = 0;
        searchState.currentIterationBestMove = Move::NULL_MOVE;
        searchState.currentIterationBestScore = neg_infinity;
        std::memset(history, 0, sizeof(history)); // set everything back to 0
        start_t = std::chrono::high_resolution_clock::now();
        // fill in piece values with our tuned values
        // a little hacky, but it works
        piece_values[0][0] = evalParams.pawn.middleGame;
        piece_values[0][1] = evalParams.knight.middleGame;
        piece_values[0][2] = evalParams.bishop.middleGame;
        piece_values[0][3] = evalParams.rook.middleGame;
        piece_values[0][4] = evalParams.queen.middleGame;
        piece_values[0][5] = 20000; // for king
        piece_values[0][6] = 0; // for empty square
        piece_values[1][0] = evalParams.pawn.endGame;
        piece_values[1][1] = evalParams.knight.endGame;
        piece_values[1][2] = evalParams.bishop.endGame;
        piece_values[1][3] = evalParams.rook.endGame;
        piece_values[1][4] = evalParams.queen.endGame;
        piece_values[1][5] = 20000; // for king
        piece_values[1][6] = 0; // for empty square
    

    }

    
    SearchState iterativeDeepening(int timeLeft, int timeIncrement, int movesToGo) {
        initSearchState();
        start_t = std::chrono::high_resolution_clock::now();
        timeForThisMove = calculateTimeForMove(timeLeft, timeIncrement, movesToGo);
        

        for (int depth = 1; depth <= MAXDEPTH; depth++) {
            int score = neg_infinity;
            
            if (stopOnThisDepth()) {
                break;
            }

            // try one aspiration window search for positions that aren't clearly winning or losing
            // if it fails, do a full window search
            if (depth > searchParams.useAspirationWindowDepth && abs(searchState.bestScore) < 300 ){
                searchState.aspirationWindow.alpha = searchState.bestScore - searchParams.aspirationWindowInitialDelta;
                searchState.aspirationWindow.beta = searchState.bestScore + searchParams.aspirationWindowInitialDelta;
                score = negamax(depth, searchState.aspirationWindow.alpha, searchState.aspirationWindow.beta, 0);

                // research on a full window if aspiration search fails
                // might need to add to the time here
                if (score <= searchState.aspirationWindow.alpha || score >= searchState.aspirationWindow.beta) {
                    score = negamax(depth, neg_infinity, infinity, 0);
                }

            }
            else{
                score = negamax(depth, neg_infinity, infinity, 0);
            }
            
            

            if (stopSearching) { // don't make updates if we're stopping
                break;
            }

            searchState.bestMove = searchState.currentIterationBestMove;
            searchState.bestScore = searchState.currentIterationBestScore;

            auto now = std::chrono::high_resolution_clock::now();
            auto dtime = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_t).count();

            std::cout << "info depth " << depth << " score cp " << searchState.bestScore << " nodes " << searchState.nodes   << " nps " << signed((searchState.nodes / (dtime + 1)) * 1000) << " time " << dtime << " pv " << uci::moveToUci(searchState.bestMove) << std::endl;
            
        }
        if (searchState.bestMove == Move::NULL_MOVE) {
            Movelist moves;
            movegen::legalmoves<MoveGenType::ALL>(moves, board);
            searchState.bestMove = moves[0];
        }
        searchState.numMovesOutofBook ++; // count only moves we've played
        return searchState;

    }

    private:
    Board& board;
    TunableSearch& searchParams;
    TunableEval& evalParams;
    Evaluator evaluator;
    TranspositionTable tt;
    SearchState searchState;

    int history[2][6][64]; // history heuristic table
    
    
    // define my own versions of infinity and negative infinity (stolen again from Sebastian Lague's chess engine tutorial)
    const int infinity = 9999999;
    const int neg_infinity = -infinity;
    const int MATE_SCORE = 10000;

    // for move ordering and piece values
    int piece_values[2][7]; // 6 piece types + empty square

    // for Time Management
    std::chrono::high_resolution_clock::time_point start_t;  // search start time
    int timeForThisMove = 0;
    bool stopSearching = false;


    bool isTimeOver() {
        // otherwise, check timeover every 2048 nodes
        if (!(searchState.nodes & 2047)) {
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

    int calculateTimeForMove(int timeRemaining, int timeIncrement, int movesToGo) {
        int timeRemainingWithOverhead = timeRemaining - 55;
        int nMoves = min(searchState.numMovesOutofBook, 10);
        int factor = 2 - nMoves / 10;
        int target = timeRemainingWithOverhead / movesToGo;
        timeForThisMove = factor * target; // offset by 50 just in case
        
        if (timeForThisMove < 50) {
            timeForThisMove =  50; // minimum time
        }
        return timeForThisMove;
    }


    // planned improvements ( do not move on until each imrpovement is tested and working):

    // add delta pruning
    // add max depth to qs search (15 is small brain's)

    int quiescence (int alpha, int beta, int ply){
        if (isTimeOver() || stopSearching) {
            return 0;
        }

        int stand_pat = evaluate(false);
        
        if (ply > MAXDEPTH){
            return stand_pat;
        }
        
        if (stand_pat >= beta) {
            return beta;
        }

        if (stand_pat > alpha) {
            alpha = stand_pat;
        }

        Movelist moves;
        movegen::legalmoves<MoveGenType::CAPTURE>(moves, board);

        scoreMoves(moves, ply);
        sortMoves(moves);

        for (const Move& move : moves) {

            // delta pruning
            // conditions:
            // 1. the move is not a promotion capture
            // 2. the move is not a check capture
            // 3. the move plus value of captured piece plus stand_pat plus margin is less than alpha
            // 4. the side to move has non-pawn material (to avoid pruning in late endgames)
            int gamePhase = evaluator.getGamePhase() > .5? 1 : 0;
            PieceType capturedPiece = board.at<PieceType>(move.to());
            int index = static_cast<int>(capturedPiece);

            // see if the move is a promotion capture or a check capture (don't apply pruning in these cases)
            int sq = static_cast<int>(move.to());
            bool isPromotion = sq < 8 || sq > 55;
            bool givesCheck = board.kingSq(board.sideToMove() == Color::WHITE? Color::BLACK : Color::WHITE) == move.to();

            if (stand_pat + piece_values[gamePhase][index] + searchParams.deltaMargin < alpha && !isPromotion && !givesCheck && board.hasNonPawnMaterial(board.sideToMove())) {
                continue;
            }

            searchState.nodes++;
            board.makeMove(move);
            int score = -quiescence(-beta, -alpha, ply + 1);
            board.unmakeMove(move);

            if (stopSearching) {
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



    int negamax(int depth, int alpha, int beta, int ply, bool nullMove = false) {
        if (isTimeOver() || stopSearching) {
            return 0;
        }
        
        // Check for draw conditions
        if (board.isRepetition() || board.halfMoveClock() >= 100 || board.isInsufficientMaterial()) {
            return 0; // Draw score (can add contempt later)
        }

        if (depth <= 0 || ply > MAXDEPTH) {
            return quiescence(alpha, beta, ply); 
        }
        

        

        bool isRoot = (ply == 0);
        bool isInCheck = board.inCheck();
        bool isPvs = beta - alpha > 1;
        int  moveCount = 0;

        
        // check extensions
        if (isInCheck){
            depth++;
        }

        // tt lookup
        uint64_t zobristKey = board.zobrist();
        optional<TTEntry> ttEntry = tt.retrieve(zobristKey);
        NodeType nodeType = NodeType::UPPERBOUND;
        Move localBestMove = Move::NULL_MOVE;
        bool useTT = false;
        Move ttMove = Move::NULL_MOVE;
        int ttScore = 0;

        if (ttEntry.has_value() && ttEntry->depth >= depth && !isRoot && !isPvs) {
            if (ttEntry->nodeType == NodeType::EXACT) {
                return ttEntry->score;
            }
            else if (ttEntry->nodeType == NodeType::LOWERBOUND) {
                alpha = max(alpha, ttEntry->score);
            }
            else if (ttEntry->nodeType == NodeType::UPPERBOUND) {
                beta = min(beta, ttEntry->score);
            }
            if (alpha >= beta) {
                return ttEntry->score;
            }
            ttMove = ttEntry->bestMove;
            ttScore = ttEntry->score;
            useTT = true;
        }

        // lazy eval option (probably bad, but we'll leave it to the tuner)
        int staticEval = evaluate(searchParams.useLazyEvalStatic);

        // reverse futility pruning or static null move pruning 
        // conditions: not a null move, not in check, not a pvs search, beta is not a mate score
        if (!isPvs && !isInCheck && (abs(beta) < (MATE_SCORE - MAXDEPTH))){
           int margin = searchParams.futilityMargin * depth * depth;
              if (staticEval - margin >= beta){
                  return staticEval - margin;
              }
        }

        // null move pruning (we pass Null Move to make sure we don't make double null moves)
        // revisit if time, because I'm not confident in this implementation
        if (!nullMove && !isPvs && !isInCheck && staticEval >= beta && depth >= 3 && board.hasNonPawnMaterial(board.sideToMove())){
            board.makeNullMove();
            int r = 2; // make sure we use at least 2 depths less for early searches (consider adding + depth / 6)
            int nullMoveScore = -negamax(depth - 1 - r, -beta, -beta + 1, ply + 1, true);
            board.unmakeNullMove();
            if (nullMoveScore >= beta){ // add a small tempo bonus 
                // make sure we don't return a false mate score
                if (nullMoveScore >= MATE_SCORE - MAXDEPTH){
                    return beta;
                }
                return nullMoveScore; // fail soft null move pruning
            }
        }

        // razoring (similar to rfp, but for alpha)
        // instead of our position being so good, it is so bad, so we prune to qs search to make sure it's bad
        if (!isPvs && !isInCheck && depth <= 3){
            int margin = searchParams.razoringMargin * depth * depth;
            if (staticEval + margin <= alpha){
                int razorScore = quiescence(alpha, beta, ply);
                if (razorScore <= alpha){
                    return alpha; // we fail to find a significant capture, so we return alpha
                }
            }
        }

        


        Movelist moves;
        movegen::legalmoves<MoveGenType::ALL>(moves, board);

        if (moves.size() == 0) {
                // Check for checkmate or stalemate
                return isInCheck ? (-MATE_SCORE + ply) : 0;
            }

        scoreMoves(moves, ply);
        sortMoves(moves);

        for (const Move& move : moves) {
            bool isCapture = board.at<PieceType>(move.to()) != PieceType::NONE;
            bool isPromotion = move.typeOf() == move.PROMOTION;

            searchState.nodes++;
            moveCount++;
            board.makeMove(move);

            int score = neg_infinity;
            

            bool doReSearch = false;


            // late move reduction
            if (depth >= searchParams.initalDepthLMR && !isPvs && !isInCheck && !isCapture && !isPromotion && moveCount > searchParams.initialMoveCountLMR) {
                int r = 1 + (log(moveCount) + log(depth)) / 1.75;
                if (r > depth - 1) {
                    r = depth - 1; // clamp to a reasonable value
                }
                score = -negamax(depth - r, -alpha - 1, -alpha, ply + 1, false);
                doReSearch = score > alpha;
            }
            else
                doReSearch = !isPvs|| moveCount > 1;

            // do a full research if lmr failed or lmr was skipped
            if (doReSearch) {
                score = -negamax(depth - 1, -alpha - 1, -alpha, ply + 1, false);
            }

            // PVS search or failed null window search
            if (isPvs && ((score > alpha && score < beta) || moveCount == 1)) {
                score = -negamax(depth - 1, -beta, -alpha, ply + 1, false);
            }
            
            board.unmakeMove(move);

            // an essential check, because otherwise we will update 
            // our searchState without a full search
            if (stopSearching) {
                return 0;
            }

             

            // we found a move that scores higher than the current best move for us
            if (score > alpha) {
                alpha = score;
                localBestMove = move;
                nodeType = NodeType::EXACT;
                

                if (isRoot) {
                    searchState.currentIterationBestMove = move;
                    searchState.currentIterationBestScore = score;
                }
                
                if (score >= beta) {
                    // update tt
                    tt.save(zobristKey, depth, beta, NodeType::LOWERBOUND, move);
                    // update killer bc beta cutoff
                    if (!isCapture && move != searchState.killerMoves[0][ply]){
                        searchState.killerMoves[1][ply] = searchState.killerMoves[0][ply];
                        searchState.killerMoves[0][ply] = move;  
                    }
                    if (!isCapture){
                    // update history bc cutoff
                    history[board.sideToMove() == Color::WHITE? 1: 0][static_cast<int>(board.at<PieceType>(move.from()))][static_cast<int>(move.to())] += depth;
                }
                    
                    return beta;
                }
            }
        }

        tt.save(zobristKey, depth, alpha, nodeType, localBestMove);
        return alpha;
    }
    


    void sortMoves(Movelist& moves) {
        std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
            return a.score() > b.score();
        });  
    }

    // score the move based on MVV/LVA
    void scoreMove(Move& move, int ply) {
        if (move == searchState.bestMove){
            move.setScore(INT16_MAX);
        }
        else if (move.typeOf() == move.PROMOTION){
            move.setScore(INT16_MAX - 1);
        }
        else if (board.at<PieceType>(move.to()) != PieceType::NONE){
            move.setScore(mvv_lva(move));
        }
        else if (move == searchState.killerMoves[0][ply]){
            move.setScore(searchParams.killerMoveScore);
        }
        else if (move == searchState.killerMoves[1][ply]){
            move.setScore(searchParams.killerMoveScore - 1);
        }
        else{
            // use history heuristic
            move.setScore(history[board.sideToMove() == Color::WHITE? 1: 0][static_cast<int>(board.at<PieceType>(move.from()))][static_cast<int>(move.to())]);
            if(move.score() > searchParams.killerMoveScore){
                move.setScore(searchParams.killerMoveScore - 2); // make sure its always less than killer move score
            }
        }
    }

    void scoreMoves(Movelist& moves, int ply){
        for (Move& move : moves){
            scoreMove(move, ply);
        }
    }

    int mvv_lva(Move& move) {
        // values are inspired by the rustic engine
        // https://rustic-chess.org/search/ordering/mvv_lva.html

        // Our enum is as follows:
        //enum class PieceType : uint8_t { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE };
        
        static const int table[6][6] = {
            {16, 15, 14, 13, 12, 11}, // pawn [0][0] = pawn captures pawn
            {26, 25, 24, 23, 22, 21}, //knight
            {36, 35, 34, 33, 32, 31}, // bishop
            {46, 45, 44, 43, 42, 41}, // rook
            {56, 55, 54, 53, 52, 51}, // queen
            {0, 0, 0, 0, 0, 0}, // king [5][0] = pawn captures king
        }; 

        int victim = static_cast<int>(board.at<PieceType>(move.to())); // piece being captured
        int aggressor = static_cast<int>(board.at<PieceType>(move.from())); // piece capturing


        // std::cout << "victim: " << victim << " aggressor: " << aggressor << std::endl;
        // std::cout << "victim piece type " << pieceTypeToChar(board.at<PieceType>(move.to())) << " aggressor piece type " << pieceTypeToChar(board.at<PieceType>(move.from())) << std::endl;

        // the multiplier is to make it more significant than the other scores
        int score = table[victim][aggressor] * 100;

        // std::cout << "score: " << score << std::endl;
        return score;
        
    }

    int evaluate(bool isLazy) {
        int eval = evaluator.evaluate(isLazy);
        if (board.sideToMove() == Color::BLACK){
            return -eval;
        }
        return eval;
    }
};