// Main function to run the UCI loop
#include <iostream>
#include <sstream>
#include <string>
#include <random>
#include <thread> // For std::thread
#include <mutex> // For std::mutex
#include <atomic> // For std::atomic
#include <memory> // For unique_ptr

// Include necessary headers from Disservin's chess library
#include "chess.hpp"
#include "searcher.hpp"

using namespace chess;
using namespace std;

chess::Board board;
unique_ptr<Searcher> searcher; // Use fully qualified name
mutex searchThreadMutex;
unique_ptr<std::thread> searchThread;
atomic<bool> stop(false);

// Function to handle the "position" command
void setPosition(Board& board, istringstream& iss) {
    string token, fen;
    iss >> token;

    if (token == "startpos") {
        fen = constants::STARTPOS;
        iss >> token; // Consume "moves" if present
    } else if (token == "fen") {
        while (iss >> token && token != "moves") {
            fen += token + " ";
        }
    }

    board.setFen(fen);

    // Apply moves if any
    while (iss >> token) {
        board.makeMove(uci::uciToMove(board, token)); // Assuming a function to convert UCI to Move
    }
}

void startSearch() {
    if (searcher) {
        lock_guard<mutex> guard(searchThreadMutex);
        if (searchThread && searchThread->joinable()) {
            searchThread->join(); // Ensure the previous search is finished
        }
        stop = false; // Reset the stop signal
        // Create a new thread for the search operation
        searchThread = make_unique<thread>([&]() {
            SearchResult result = searcher->search(5); // Start search with depth 6
            cout << "bestmove " << uci::moveToUci(result.bestMove)
                 << " info depth " << result.depth << " score cp " << result.score << endl;
        });
    }
}

void stopSearch() {
    stop = true; // Signal the search to stop
    lock_guard<mutex> guard(searchThreadMutex);
    if (searchThread && searchThread->joinable()) {
        searchThread->join(); // Wait for the search to finish
    }
}

int main() {
    searcher = make_unique<Searcher>(board); // Use fully qualified name
    string line, token;

    while (getline(cin, line)) {
        istringstream iss(line);
        iss >> token;

        if (token == "uci") {
            cout << "id name MyChessEngine" << endl;
            cout << "id author My Name" << endl;
            // Output other UCI options here, if any
            cout << "uciok" << endl;
        } else if (token == "isready") {
            cout << "readyok" << endl;
        } else if (token == "position") {
            setPosition(board, iss);
        } else if (token == "go") {
            startSearch();
        } else if (token == "stop") {
            stopSearch();
        } else if (token == "quit") {
            stopSearch(); // Ensure the search thread is properly joined before quitting
            break; // Exit the loop and end the program
        }
        // Handle other UCI commands as needed
    }

    return 0; // searcher is automatically deleted due to unique_ptr
}
