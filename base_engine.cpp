// Main function to run the UCI loop
#include <iostream>
#include <sstream>
#include <string>
#include <random>

// Include necessary headers from Disservin's chess library
#include "chess.hpp"
#include "searcher.hpp"

using namespace chess;
using namespace std;

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

// Function to get a random move from the current position
void getRandomMove(const chess::Board& board) {
    Movelist moves;
    movegen::legalmoves(moves, board);

    if (!moves.empty()) {
        uniform_int_distribution<> dis(0, moves.size() - 1);
        static mt19937 gen(random_device{}());
        Move random_move =  moves[dis(gen)];
        cout << "bestmove " << uci::moveToUci(random_move) << endl;
    } else {
        // No legal moves, should signal a checkmate or stalemate
        cout << "bestmove 0000" << endl; // 0000 is used to signal no move (game over situation)
    }
}

int main() {
    Board board;
    string line, token;
    Searcher searcher(board);

    while (getline(cin, line)) {
        istringstream iss(line);
        iss >> token;

        if (token == "uci") {
            cout << "id name current" << endl;
            cout << "id author Elliot Harris" << endl;
            cout << "uciok" << endl;
        } else if (token == "isready") {
            cout << "readyok" << endl;
        } else if (token == "ucinewgame") {
            board.setFen(constants::STARTPOS);
        } else if (token == "position") {
            setPosition(board, iss);
        } else if (token == "go") {
            searcher.search(4);
        } else if (token == "quit") {
            break;
        }

    // Implement other UCI commands as needed

    }

    return 0;
}


