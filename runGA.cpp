#include "ga_util.hpp"
#include "ga.hpp"
#include "chess.hpp"
#include <string>
#include <cassert> // Add missing include for assert

// for now just going to test my utility functions
int main() {
    // TunableEval randomEvals = initializeRandomTunableEval();
    // std::string randomChromosone = convertEvalToChromosone(randomEvals);
    // TunableEval randomEvalsClone = convertChromosoneToEval(randomChromosone);

    // // assert to see if they are the same
    // assert(randomEvals.pawn.endGame == randomEvalsClone.pawn.endGame / 10); 
    // assert(randomEvals.kingPressureScore.endGame == randomEvalsClone.kingPressureScore.endGame);

    // values from 2008 paper
    // population size = 1000
    // crossover rate = 0.75
    // mutation rate = 0.002
    // number of generations = 300
    // vars to time the algorithm
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();



    double initialMutationRate = 0.1;
    double decayRate = 0.85;
    double crossoverRate = 0.75;
    int totalGenerations = 300;
    size_t populationSize = 2000; 
    int trainingSize = 2000;
    int eliteSize = 50;
    int archiveSize = 100;
    int replacementCount = 50;
    std::string trainingDataPath = "dbs/quiet_evals_filtered.csv";

    std::cout << "Running Genetic Algorithm" << std::endl;
    std::cout << "Population Size: " << populationSize << std::endl;
    std::cout << "Initial Mutation Rate: " << initialMutationRate << std::endl;
    std::cout << "Mutation Decay Rate: " << decayRate << std::endl;
    std::cout << "Crossover Rate: " << crossoverRate << std::endl;
    std::cout << "Total Generations: " << totalGenerations << std::endl;
    std::cout << "Training Size: " << trainingSize << std::endl;
    std::cout << "Elite Size: " << eliteSize << std::endl;
    std::cout << "Archive Size: " << archiveSize << std::endl;
    std::cout << "Replacement Count: " << replacementCount << std::endl;

    
    GeneticAlgorithm GA = GeneticAlgorithm(populationSize, initialMutationRate, decayRate, crossoverRate, totalGenerations, trainingSize, eliteSize, archiveSize, replacementCount, trainingDataPath);
    GA.run();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " seconds" << std::endl;
    return 0;
}
