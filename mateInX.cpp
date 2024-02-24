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
#include <atomic>

std::atomic<bool> stop(false);


int main() {
    // Map of FEN strings to their best moves
    std::map<std::string, std::string> fenToBestMove = {
        //starting position, for testing branching factor, etc.
        {chess::constants::STARTPOS, "e2e4"},
        
        // mate in x series: https://lichess.org/study/0uBy1QsD
        {"4r2k/1p3rbp/2p1N1p1/p3n3/P2NB1nq/1P6/4R1P1/B1Q2RK1 b - - 4 32", "h4h2"}, // in 1
        {"4rr1k/1p4bp/2p3p1/p7/P2pBQn1/1P3nqN/6P1/B3RR1K b - - 0 33", "f8f4"}, // in 2
        {"4r2k/1p3rbp/2p3p1/p7/P2pB1nq/1P3n1N/6P1/B1Q1RR1K b - - 3 31", "h4g3"}, // in 3
        {"3qr2k/1p3rbp/2p3p1/p7/P2pBNn1/1P3n2/6P1/B1Q1RR1K b - - 1 30", "d8h4"}, //in 4
        {"4rb1k/2pqn2p/6pn/ppp3N1/P1QP2b1/1P2p3/2B3PP/B3RRK1 w - - 0 24", "f1f8"}, //in 5
        {"1n3k2/4r1b1/rp1p1n2/5p1p/3P4/Pq2P1RP/1B1N1P1R/1K6 b - - 1 30", "b3d1"}, // in 6

        // tactical non mate series
        {"2r5/ppp3B1/4kp1R/8/5P2/P7/1n5P/6K1 b - - 3 29", "c8g8"}, // 1100 elo puzzle
        {"rnbqr1k1/ppp2p1p/3p1bPB/8/4P1Q1/1BN5/PPP3PP/R3K2R b KQ - 0 15", "c8g4"}, // 1200 elo puzzle
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", "e2a6"}, // perft2 position


        {"3r1r1b/ppq2p1k/2p1p1p1/4Nn1n/2PP1P1p/1PQ2R1P/PB2N1P1/3R2K1 b - - 0 1", "h5f4"},  // estimated 2500+ elo puzzle
        {"1r3r2/4bpkp/1qb1p1p1/3pP1P1/p1pP1Q2/PpP2N1R/1Pn1B2P/3RB2K w - - 0 1", "f4f6"}, // estimated 3000+ elo puzzle


        //endgame series
        
        // {"8/8/8/8/p1k5/P1p4p/2K4P/8 w - - 0 61", "c2c1"} // thought for an eternity, couldnt crack it, explosion at depth 18

        // Add all other FEN strings and their best moves here
    };

    // Create a new Board object
    Board board;
    Searcher searcher(board); // Create a new Searcher object

    long totalTotalNodes = 0;
    long totalTotalTime = 0;

    for (const auto& [fen, bestMove] : fenToBestMove) {
        board.setFen(fen); // Initialize the board with the FEN string

        auto startTime = std::chrono::high_resolution_clock::now();

        Move b = searcher.search(4000, 0, 1); // Adjust parameters as needed, should take one second to reach max depth, 
        // then finish search on that depth

        SearchState sMetrics = searcher.getState();

        auto endTime = std::chrono::high_resolution_clock::now();
        auto searchDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

        std::string suggestedMove = uci::moveToUci(b); // Convert the suggested move to UCI format

        // Access the searcher's metrics
        unsigned long long totalNodes = sMetrics.nodes;
        int depthReached = sMetrics.currentDepth;

        // Reporting
        std::cout << "Position: " << fen << std::endl;
        std::cout << "Suggested Move: " << suggestedMove << " | Best Move: " << bestMove << std::endl;
        std::cout << "Search Time: " << searchDuration << " ms" << std::endl;
        std::cout << "Total Nodes: " << totalNodes << std::endl;
        std::cout << "Depth Reached: " << depthReached << std::endl << std::endl;

        totalTotalNodes += totalNodes;
        totalTotalTime += searchDuration;

        // Reset metrics and search parameters as necessary
        stop = false; // Reset the stop flag if used for search termination
        searcher.clear(); // Clear the TT table if necessary
}

    std::cout << "Total Total Nodes: " << totalTotalNodes << std::endl;
    std::cout << "Total Total Time: " << totalTotalTime << " ms" << std::endl;
    std::cout << "Average Nodes per second: " << (totalTotalNodes / (totalTotalTime / 1000)) << std::endl;

    return 0;
}









