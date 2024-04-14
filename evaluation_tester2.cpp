#include "evaluator.hpp"
#include "chess.hpp"
#include "baselines.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>



int main() {
    Board board = Board();
    Evaluator evaluator = Evaluator(board, baseEval);

    std::string positionsPath = "dbs/evaluations_test.csv";

    std::ifstream positionsFile(positionsPath);
    if (!positionsFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    std::string line;
    // Read and discard the header row
    getline(positionsFile, line);

    // Process each line, which includes FEN and its associated eval
    while (getline(positionsFile, line)) {
        std::stringstream ss(line);
        std::string fen;
        std::string originalEvalStr;
        int originalEval;

        getline(ss, fen, ','); // Read FEN
        getline(ss, originalEvalStr); // Read evaluation
        originalEval = std::stoi(originalEvalStr); // Convert string to integer

        board.setFen(fen);
        int computedEval = evaluator.evaluate();
        

        if (fen == "2Rb2k1/3q1rpp/p3Q3/2N1p3/4P3/3Pp3/1P4PP/1K6 b - - 1 2"){
            std::cout << "Original eval: " << originalEval << std::endl;
            std::cout << "Computed eval: " << computedEval << std::endl;
        }
        // Assert the computed evaluation matches the original evaluation
        assert(computedEval == originalEval);
    }

    positionsFile.close();
    std::cout << "All evaluations match the original data." << std::endl;

    return 0;
}
