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
    double mutationRate = 0.002;
    double crossoverRate = 0.75;
    int totalGenerations = 300;
    size_t populationSize = 500; // at around 150 seconds per generation,
    int trainingSize = 1000;
    
    GeneticAlgorithm GA = GeneticAlgorithm(populationSize, mutationRate, crossoverRate, totalGenerations, trainingSize);
    GA.run();
    return 0;
}
