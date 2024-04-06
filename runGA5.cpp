#include "ga_util.hpp"
#include "ga5.hpp"
#include "chess.hpp"
#include <string>
#include <iostream>
#include <chrono>
#include "logger.hpp"
#include <cassert> // Add missing include for assert

// for now just going to test my utility functions
int main() {

    // values from 2008 paper
    // population size = 1000
    // crossover rate = 0.75
    // mutation rate = 0.002
    // number of generations = 300
    // vars to time the algorithm
    Logger::getInstance().openLogFile("ga_log.txt");
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();



    double initialMutationRate = 0.02;
    double decayRate = 0;
    double crossoverRate = 0.75;
    int totalGenerations = 200;
    size_t populationSize = 20; // because we have twenty threads, we will almost always have a pop size of at least 20. This corresponds to each thread playing 100 games per generation (still a lot less than 19X20 )
    int eliteSize = 0;
    int archiveSize = 400;
    int replacementCount = 4; // we can be more aggresive since we are deleting stuff after

    Logger::getInstance().log(  "Running Genetic Algorithm"  );
    Logger::getInstance().log(  "Population Size: " + std::to_string(populationSize)  );
    Logger::getInstance().log(  "Initial Mutation Rate: " + std::to_string(initialMutationRate)  );
    Logger::getInstance().log(  "Mutation Decay Rate: " + std::to_string(decayRate)  );
    Logger::getInstance().log(  "Crossover Rate: " + std::to_string(crossoverRate)  );
    Logger::getInstance().log(  "Total Generations: " + std::to_string(totalGenerations)  );
    Logger::getInstance().log(  "Elite Size: " + std::to_string(eliteSize)  );
    Logger::getInstance().log(  "Archive Size: " + std::to_string(archiveSize)  );
    Logger::getInstance().log(  "Replacement Count: " + std::to_string(replacementCount)  );


    GA5 GA = GA5(populationSize, initialMutationRate, decayRate, crossoverRate, totalGenerations,  eliteSize, archiveSize, replacementCount);
    GA.run();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    Logger::getInstance().log("Time taken: " + std::to_string( std::chrono::duration_cast<std::chrono::seconds>(end - begin).count()) + " seconds" );
    Logger::getInstance().closeLogFile();
    return 0;
}