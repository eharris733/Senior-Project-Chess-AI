#include "ga_util.hpp"
#include "ga3.hpp"
#include "chess.hpp"
#include <string>
#include <iostream>
#include <chrono>
#include "logger.hpp"
#include <cassert> // Add missing include for assert

// for now just going to test my utility functions
int main() {
    TunableEval randomEvals = initializeRandomTunableEval();
    std::string randomChromosone = convertEvalToChromosone(randomEvals);
    TunableEval randomEvalsClone = convertChromosoneToEval(randomChromosone);

    // assert to see if they are the same
    assert(randomEvals.pawn.endGame == randomEvalsClone.pawn.endGame); 
    assert(randomEvals.kingPressureScore.endGame == randomEvalsClone.kingPressureScore.endGame);

    // values from 2008 paper
    // population size = 1000
    // crossover rate = 0.75
    // mutation rate = 0.002
    // number of generations = 300
    // vars to time the algorithm
    Logger::getInstance().openLogFile("ga_log.txt");
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();



    double initialMutationRate = 0.07;
    double decayRate = 0.02;
    double crossoverRate = 0.75;
    int totalGenerations = 200;
    size_t populationSize = 20; // because we have twenty threads, we will almost always have a pop size of at least 20
    int eliteSize = 4;
    int archiveSize = 40;
    int replacementCount = 2;

    Logger::getInstance().log(  "Running Genetic Algorithm"  );
    Logger::getInstance().log(  "Population Size: " + std::to_string(populationSize)  );
    Logger::getInstance().log(  "Initial Mutation Rate: " + std::to_string(initialMutationRate)  );
    Logger::getInstance().log(  "Mutation Decay Rate: " + std::to_string(decayRate)  );
    Logger::getInstance().log(  "Crossover Rate: " + std::to_string(crossoverRate)  );
    Logger::getInstance().log(  "Total Generations: " + std::to_string(totalGenerations)  );
    Logger::getInstance().log(  "Elite Size: " + std::to_string(eliteSize)  );
    Logger::getInstance().log(  "Archive Size: " + std::to_string(archiveSize)  );
    Logger::getInstance().log(  "Replacement Count: " + std::to_string(replacementCount)  );


    GA3 GA = GA3(populationSize, initialMutationRate, decayRate, crossoverRate, totalGenerations,  eliteSize, archiveSize, replacementCount);
    GA.run();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    Logger::getInstance().log("Time taken: " + std::to_string( std::chrono::duration_cast<std::chrono::seconds>(end - begin).count()) + " seconds" );
    Logger::getInstance().closeLogFile();
    return 0;
}