// Main function to run the UCI loop
#include <iostream>
#include <sstream>
#include <string>
#include <random>
#include <thread> // For std::thread
#include <mutex> // For std::mutex
#include <memory> // For unique_ptr
#include <atomic> // For std::atomic
// from my code
#include "chess.hpp"
#include "searcher.hpp"

using namespace chess;
using namespace std;

chess::Board board;
unique_ptr<Searcher> searcher; 
mutex searchThreadMutex;
unique_ptr<std::thread> searchThread;


void setPosition(const std::string& uci, const std::vector<std::string>& tokens) {
    int ntokens = tokens.size();

    // starting position
    if (tokens[1] == "startpos"){
        board.setFen(constants::STARTPOS);
        // reset the tt
        searcher->clear();
    }
    else if (tokens[1] == "fen")
        board.setFen(uci.substr(13));   // get text after "fen"

    int i = 1;
    while (++i<ntokens)
        if (tokens[i] == "moves")
            break;
    
    // play moves
    while (++i<ntokens)
        board.makeMove(chess::uci::uciToMove(board, tokens[i]));
}

void startSearch(int timeLeft, int timeIncrement, int movesToGo) {
    if (searcher) {
        lock_guard<mutex> guard(searchThreadMutex);
        if (searchThread && searchThread->joinable()) {
            searchThread->join(); // Ensure the previous search is finished
        }

        // Create a new thread for the search operation
        // Capture time control parameters by value in the lambda
        searchThread = make_unique<thread>([=]() {
            cout << "timeLeft: " << timeLeft << " timeIncrement: " << timeIncrement << " movesToGo: " << movesToGo << endl;
            SearchState result = searcher->search(timeLeft, timeIncrement, movesToGo); 
            cout << "bestmove " << uci::moveToUci(result.bestMove) << endl;
        });
    }
}


void stopSearch() {
    lock_guard<mutex> guard(searchThreadMutex);
    if (searchThread && searchThread->joinable()) {
        searchThread->join(); // Wait for the search to finish
    }
}

// Splits strings into words seperated by delimiter, stolen from 
//https://github.com/Orbital-Web/Raphael/blob/main/uci.cpp#L23
vector<string> splitstr(const std::string& str, const char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (getline(ss, token, delim)) 
        tokens.push_back(token);

    return tokens;
}

int main() {
    searcher = make_unique<Searcher>(board, baseSearch, ga1result10); // Use fully qualified name
    string uci;
    bool quit = false;
    bool isWhiteTurn = board.sideToMove() == Color::WHITE;

while (!quit) {
    // Get input
    getline(std::cin, uci);
    auto tokens = splitstr(uci, ' ');
    auto keyword = tokens[0];

    // Parse input
    if (keyword == "uci") {
        cout << "id name Gerald Current" << endl;
        cout << "id author Elliot Harris" << endl;
        // Output other UCI options here, if any
        cout << "uciok" << endl;
    } 
    // else if (keyword == "setoption") { // don't have a setoption 
    //     // handle setoption command
    // }
    else if (keyword == "isready") {
        cout << "readyok" << endl;
    } 
    else if (keyword == "position") {
        setPosition(uci, tokens);
        isWhiteTurn = board.sideToMove() == Color::WHITE; // Update isWhiteTurn
    } 
    else if (keyword == "go") {
    int timeLeft = 0;
    int timeIncrement = 0;
    int movesToGo = 30; // Default value

    for (size_t i = 1; i < tokens.size(); ++i) {
        std::cout << "Parsing token: " << tokens[i] << std::endl; // Debug print

        if (tokens[i] == "wtime" && isWhiteTurn) {
            timeLeft = std::stoi(tokens[i + 1]);
        } else if (tokens[i] == "btime" && !isWhiteTurn) {
            timeLeft = std::stoi(tokens[i + 1]);
        } else if (tokens[i] == "winc" && isWhiteTurn) {
            timeIncrement = std::stoi(tokens[i + 1]);
        } else if (tokens[i] == "binc" && !isWhiteTurn) {
            timeIncrement = std::stoi(tokens[i + 1]);
        } else if (tokens[i] == "movestogo") {
            movesToGo = std::stoi(tokens[i + 1]);
        } else if (tokens[i] == "movetime") {
            timeLeft = std::stoi(tokens[i + 1]);
            movesToGo = 1;
        }
        // Debug output
        std::cout << "timeLeft: " << timeLeft << " timeIncrement: " << timeIncrement << " movesToGo: " << movesToGo << std::endl;
    }

    // Start the search with time management
    startSearch(timeLeft, timeIncrement, movesToGo);
    }
    else if (keyword == "quit") {
        quit = true;
    }
}


    return 0; // searcher is automatically deleted due to unique_ptr
}
