// Main function to run the UCI loop
#include <iostream>
#include <sstream>
#include <string>
#include <random>

// Include necessary headers from Disservin's chess library
#include "chess.hpp"
using namespace chess;
using namespace std;

// Function to handle the "position" command
void setPosition(chess::Board& board, std::istringstream& iss) {
    std::string token, fen;
    iss >> token;

    if (token == "startpos") {
        fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
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
    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);

    if (!moves.empty()) {
        std::uniform_int_distribution<> dis(0, moves.size() - 1);
        static std::mt19937 gen(std::random_device{}());
        Move random_move =  moves[dis(gen)];
        cout << "bestmove " << uci::moveToUci(random_move) << endl;
    } else {
        // No legal moves, should signal a checkmate or stalemate
        std::cout << "bestmove 0000" << std::endl; // 0000 is used to signal no move (game over situation)
    }
}

int main() {
    chess::Board board;
    std::string line, token;

    while (getline(std::cin, line)) {
        std::istringstream iss(line);
        iss >> token;

        if (token == "uci") {
            std::cout << "id name RandomChess" << std::endl;
            std::cout << "id author YourName" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (token == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (token == "ucinewgame") {
            board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        } else if (token == "position") {
            setPosition(board, iss);
        } else if (token == "go") {
            getRandomMove(board);
        } else if (token == "quit") {
            break;
        }

    // Implement other UCI commands as needed

    }

    return 0;
}


