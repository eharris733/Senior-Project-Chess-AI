#include <iostream>
#include <string>
#include <vector>
#include <random>

// Include the necessary headers from Disservin's chess library
#include "chess.hpp"

using namespace chess;
using namespace std;




int main() {
    std::string line;
    Board board;

    while (getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "uci") {
            std::cout << "id name RandomChess" << std::endl;
            std::cout << "id author YourName" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (token == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (token == "position") {
            // Process the position command
            std::string fen;
            iss >> token; // "startpos" or "fen"
            if (token == "startpos") {
                board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            } else if (token == "fen") {
                getline(iss, fen); // Get the rest of the line containing the FEN
                board.setFen(fen);
            }
        } else if (token == "go") {
            Movelist moves;
            movegen::legalmoves(moves, board);

            Move random_move = moves[0];
            std::cout << "bestmove " << uci::moveToUci(random_move) << std::endl;
        } else if (token == "quit") {
            break;
        }
    }

    return 0;
}
