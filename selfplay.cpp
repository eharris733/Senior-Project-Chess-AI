#include <atomic>
#include "searcher.hpp"
#include "chess.hpp"
#include <cstdlib>
#include <ctime>
#include <map>
#include <atomic>
#include <iostream>
#include <algorithm>
#include <chrono> 

std::atomic<bool> stop(false);




// Function to simulate a game between two searchers
int simulateGame(Searcher& whiteSearcher, Searcher& blackSearcher, Board& board) {
    int result = 0;
    board.setFen(constants::STARTPOS); // Set the board to the starting position
    whiteSearcher.setMaxDepth(6); // Set the search depth for white
    blackSearcher.setMaxDepth(6); // Set the search depth for black
    whiteSearcher.setVerbose(false); // Disable verbose output for white
    blackSearcher.setVerbose(false); // Disable verbose output for black
    while (board.isGameOver().second == GameResult::NONE) {
        Move move;

        if (board.sideToMove() == Color::WHITE) {
            move = whiteSearcher.search(1000, 0, 1).bestMove; //100ms, 0ms increment, 1 move to go
        } else {
            move = blackSearcher.search(1000, 0, 1).bestMove; // should reach depth 6 first, but just in case
        }

        board.makeMove(move); // Assume makeMove applies the move to the board
        
    }
    // Check for game end conditions    
        if (board.isGameOver().second == GameResult::LOSE) {
            result = board.sideToMove() == Color::WHITE ? -1: 1;
        } else if (board.isGameOver().second == GameResult::DRAW){
            result = 0;
        }
        

    return result;
}

// Example usage
int main() {
    Board board; // Initialize the board object
    Searcher whiteSearcher = Searcher(board);
    // Configure searchers if needed...
    Searcher blackSearcher = Searcher(board);

    int gameResult = simulateGame(whiteSearcher, blackSearcher, board);

    // Output the result
    switch (gameResult) {
        case 1:
            std::cout << "White wins!\n";
            break;
        case -1:
            std::cout << "Black wins!\n";
            break;
        case 0:
            std::cout << "The game is a draw.\n";
            break;
        default:
            std::cout << "Game ongoing or error occurred.\n";
    }

    return 0;
}
