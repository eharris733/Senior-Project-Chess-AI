// "4r2k/1p3rbp/2p1N1p1/p3n3/P2NB1nq/1P6/4R1P1/B1Q2RK1 b - - 4 32",
// "4r3/1pp2rbk/6pn/4n3/P3BN1q/1PB2bPP/8/2Q1RRK1 b - - 0 31",
// "4r1k1/4r1p1/8/p2R1P1K/5P1P/1QP3q1/1P6/3R4 b - - 0 1",
// "3qr2k/1p3rbp/2p3p1/p7/P2pBNn1/1P3n2/6P1/B1Q1RR1K b - - 1 30",
// "4rb1k/2pqn2p/6pn/ppp3N1/P1QP2b1/1P2p3/2B3PP/B3RRK1 w - - 0 24",
// "4rr2/1p4bk/2p3pn/B3n2b/P4N1q/1P5P/6PK/1BQ1RR2 b - - 1 31",
// "5r2/1pB2rbk/6pn/4n2q/P3B3/1P5P/4R1P1/2Q2R1K b - - 3 33",
// "4r2k/1p3rbp/2p1N1pn/p3n3/P2NB3/1P4q1/4R1P1/B1Q2RK1 b - - 4 32",
// "2r5/1pB2Rbk/6pn/4n1q1/P3B3/1P5P/6P1/2Q1R2K w - - 1 34",
// "8/1pB1rnbk/6pn/7q/P3B2P/1P6/6P1/2Q1R2K b - - 0 34",
// "rn2qrk1/p4p2/1p2p2Q/3pP3/PbbNn3/5NP1/1P3PBP/R3R1K1 w - - 1 19",
// "R4r1k/6pp/2p5/2n2b2/2Q1pP1b/1r2P2B/NP4KP/2Bq2NR b - - 3 25"


#include <iostream>
#include <map>
#include <string>
#include "chess.hpp" // Include your Board class header
#include "searcher.hpp" // Include your Searcher class header

int main() {
    // Map of FEN strings to their best moves
    std::map<std::string, std::string> fenToBestMove = {
        {"4r2k/1p3rbp/2p1N1p1/p3n3/P2NB1nq/1P6/4R1P1/B1Q2RK1 b - - 4 32", "h4h2"},
        {"4r3/1pp2rbk/6pn/4n3/P3BN1q/1PB2bPP/8/2Q1RRK1 b - - 0 31", "h4g3"},
        {"4r1k1/4r1p1/8/p2R1P1K/5P1P/1QP3q1/1P6/3R4 b - - 0 1", "g7g6"},
        // Add all other FEN strings and their best moves here
    };


    for (const auto& [fen, bestMove] : fenToBestMove) {
        Board board;
        board.setFen(fen); // Initialize the board with the FEN string
        Searcher searcher(board);

        // Perform the search to get the best move. Adjust based on your implementation.
        Move move = searcher.search(3); // This should be adjusted to match your implementation

        // Assuming Move.toString() converts the move to a string. Adjust as necessary.
        std::string suggestedMove = uci::moveToUci(move); // Convert the suggested move to a string

        // Check if the suggested move matches the known best move
        if (suggestedMove == bestMove) {
            std::cout << "Correct move for position: " << fen << std::endl;
        } else {
            std::cout << "Incorrect move for position: " << fen << std::endl;
            std::cout << "Suggested move: " << suggestedMove << " | Best move: " << bestMove << std::endl;
        }
    }

    return 0;
}









