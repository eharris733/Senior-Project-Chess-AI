#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "nnue.h" // Include your SmallBrain and related headers
#include "board.h" 
#include "evaluation.h"
#include "uci.h"



std::vector<std::string> readCSV(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        std::vector<std::string> positions;

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string fen;
            double score;
            if (std::getline(ss, fen, ',') && ss >> score) {
                positions.push_back(fen);
            }
        }
        return positions;
    }  


int main() {
    nnue::init(""); // Initialize the NNUE neural network
    Board board; // Create a board object
    std::vector<std::string> positions = readCSV("/home/eh5954/Desktop/Senior-Project-Chess-AI/dbs/mastergames/random_positions_no_duplicates.csv");

    // Specify the path to your output CSV file
    std::ofstream outFile("/home/eh5954/Desktop/Senior-Project-Chess-AI/dbs/smallbrain_evals.csv");
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file." << std::endl;
        return 1; // Or handle the error as appropriate
    }

    for (const auto& position : positions) {
        board.setFen(position);
        int score = eval::evaluate(board); // Get the evaluation score
        std::string evalStr = uci::convertScore(score); // Convert the score to a string

        // Write the FEN string and its evaluation to the output file
        outFile << position << "," << evalStr << std::endl;
    }

    outFile.close(); // Don't forget to close the file

    return 0;
}
