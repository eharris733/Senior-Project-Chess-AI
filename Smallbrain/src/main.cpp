#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "nnue.h" // Include your SmallBrain and related headers
#include "board.h" 
#include "evaluation.h"
#include "uci.h"



// Assuming 'Board' and 'eval' namespaces are correctly set up as per your project

void updateEvals(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream inFile(inputFile);
    std::ofstream outFile(outputFile);

    if (!inFile.is_open() || !outFile.is_open()) {
        std::cerr << "Error opening files!" << std::endl;
        return;
    }

    nnue::init(""); // Initialize the NNUE neural network

    std::string line;
    while (getline(inFile, line)) {
        std::stringstream ss(line);
        std::string fen, oldEval;
        getline(ss, fen, ','); // Assuming FEN and EVAL are separated by a comma
        getline(ss, oldEval);  // This value is ignored since we'll calculate a new evaluation

        Board board = Board(fen); // Set up the board with the FEN string
        int newEval = eval::evaluate(board); // Evaluate the board position
        
        // Convert the evaluation score to a string representation, if necessary
        std::string newEvalStr = uci::convertScore(newEval); // Assuming you have implemented this function

        outFile << fen << "," << newEvalStr << std::endl; // Write the updated FEN and EVAL to the output file
    }

    inFile.close();
    outFile.close();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_csv_file> <output_csv_file>" << std::endl;
        return 1;
    }

    updateEvals(argv[1], argv[2]);

    return 0;
}
