#include <iostream>
#include <map>
#include <string>
#include <atomic>
#include <chrono>
#include "../engine/chess.hpp" // Include your Board class header
#include "../engine/searcher2.hpp" 
#include "../engine/ga3and5results.hpp"
#include "../engine/ga1results.hpp"



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
    Searcher2 searcher(board, resultX, ga1result10); // Create a new Searcher object

    long totalTotalNodes = 0;
    long totalTotalTime = 0;

    for (const auto& [fen, bestMove] : fenToBestMove) {
        board.setFen(fen); // Initialize the board with the FEN string

        auto startTime = std::chrono::high_resolution_clock::now();

        SearchState sMetrics = searcher.iterativeDeepening(1000, 0, 1); // Adjust parameters as needed, should take one second to reach max depth, 
        Move m = sMetrics.bestMove;
        // then finish search on that depth


        auto endTime = std::chrono::high_resolution_clock::now();
        auto searchDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

        std::string suggestedMove = uci::moveToUci(m); // Convert the suggested move to UCI format

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
        searcher.clear(); // Clear the TT table if necessary
}

    std::cout << "Total Total Nodes: " << totalTotalNodes << std::endl;
    std::cout << "Total Total Time: " << totalTotalTime << " ms" << std::endl;
    std::cout << "Average Nodes per second: " << (totalTotalNodes / (totalTotalTime / 1000)) << std::endl;

    return 0;
}









