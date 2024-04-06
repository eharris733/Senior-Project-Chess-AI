#include "chess.hpp"
#include "baselines.hpp"
#include "evaluator.hpp"

#include <chrono>

using namespace chess;
using namespace std;

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
    // Move killerMoves[2]; // store two killer moves
    int currentDepth = 1;
    long nodes = 0;
    // AspirationWindow aspirationWindow = AspirationWindow();
    // bool isOpening = true;
    int numMovesOutofBook = 0;
};

class Searcher2 {
    public:
    Searcher2(Board& initialBoard, TunableSearch searchParams = baseSearch, TunableEval evalParams = baseEval)
    : board(initialBoard), searchParams(searchParams), evalParams(evalParams), evaluator(board, evalParams){
    }

    
    SearchState iterativeDeepening(int timeLeft, int timeIncrement, int movesToGo) {
        searchState.bestMove = Move::NULL_MOVE;
        searchState.bestScore = 0;
        searchState.currentIterationBestMove = Move::NULL_MOVE;
        start_t = std::chrono::high_resolution_clock::now();

        for (int depth = 1; depth <= MAXDEPTH; depth++) {
            if (stopOnThisDepth()) {
                break;
            }
            searchState.currentDepth = depth;
            searchState.currentIterationBestScore = negamax(depth, neg_infinity, infinity, 0);
            searchState.bestScore = searchState.currentIterationBestScore;
            searchState.bestMove = searchState.currentIterationBestMove;
            
        }

        searchState.numMovesOutofBook ++; // count only moves we've played
        return searchState;

    }

    private:
    Board board;
    TunableSearch searchParams;
    TunableEval evalParams;
    Evaluator evaluator;
    SearchState searchState;
    int MAXDEPTH = 100;
    // define my own versions of infinity and negative infinity (stolen again from Sebastian Lague's chess engine tutorial)
    const int infinity = 9999999;
    const int neg_infinity = -infinity;

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

    void calculateTimeForMove(int timeRemaining, int timeIncrement, int movesToGo) {
        int timeRemainingWithOverhead = timeRemaining - 55;
        int nMoves = min(searchState.numMovesOutofBook, 10);
        int factor = 2 - nMoves / 10;
        int target = timeRemainingWithOverhead / movesToGo;
        timeForThisMove = factor * target; // offset by 50 just in case
        
        if (timeForThisMove < 50) {
            timeForThisMove =  50; // minimum time
        }
    }

    int negamax(int depth, int alpha, int beta, int ply) {
        if (depth == 0) {
            return evaluate();
        }
        // Check for draw conditions
        if (board.isRepetition() || board.halfMoveClock() >= 100 || board.isInsufficientMaterial()) {
            return 0; // Draw score (can add contempt later)
        }

        if (isTimeOver()) {
            return 0;
        }

        searchState.nodes++;

        Movelist moves;
        movegen::legalmoves<MoveGenType::ALL>(moves, board);

        for (const Move& move : moves) {
            board.makeMove(move);
            int score = -negamax(depth - 1, -beta, -alpha, ply + 1);
            board.unmakeMove(move);

            // an essential check, because otherwise we will update 
            // our state without a full search
            if (stopSearching) {
                return 0;
            }

            

            // we found a move that scores higher than the current best move for us
            if (score > alpha) {
                alpha = score;
                searchState.currentIterationBestScore = score;
                searchState.currentIterationBestMove = move;
                
                
                if (score >= beta) {
                    return beta;
                }
            }
        }

        return alpha;

    }

    int evaluate() {
        int eval = evaluator.evaluate(0);
        if (board.sideToMove() == Color::BLACK){
            return -eval;
        }
        return eval;
    }
};