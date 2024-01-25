 #include "chess.hpp"
 #include "evaluator.hpp"
#include <cstdlib>

 using namespace chess;

 int main(){

Board board = Board(constants::STARTPOS); //starting position
Evaluator evaluator = Evaluator(board);
std::cout << "evaluation for starting position: " << evaluator.evaluate(0) << std::endl;

board.setFen("8/3kb2n/3p2p1/1N6/1Pp1NQP1/2P5/5PK1/4q3 b - - 0 48"); // should be medium advantage for white in the early endgame / late middlegame
std::cout << "evaluation for position 2: " << evaluator.evaluate(0) << std::endl; 

board.setFen("3r2k1/1p3p2/2p2bp1/8/p1qP1P2/2B1Q1P1/P1B4P/6K1 w - - 3 34"); // should be medium advantage for black in the early endgame / late middlegame
std::cout << "evaluation for position 3: " << evaluator.evaluate(0) << std::endl;

board.setFen("r2q1rk1/1b3ppp/4p3/pNb1P3/8/PB1Q4/1PP3PP/R4R1K b - - 1 18");
std::cout << "evaluation for position 4: " << evaluator.evaluate(0) << std::endl;

    return 0;

 }