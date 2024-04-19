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
    
    TunableSearch randomSearch = initializeRandomTunableSearch();
    std::string randomChromosone = convertSearchToChromosome(randomSearch);
    TunableSearch randomSearchClone = convertChromosomeToSearch(randomChromosone);

    // assert to see if they are the same
    assert(randomSearch.aspirationWindowInitialDelta == randomSearchClone.aspirationWindowInitialDelta);
    assert(randomSearch.useAspirationWindowDepth == randomSearchClone.useAspirationWindowDepth);
    assert(randomSearch.deltaMargin == randomSearchClone.deltaMargin);
    assert(randomSearch.futilityMargin == randomSearchClone.futilityMargin);
    assert(randomSearch.razoringMargin == randomSearchClone.razoringMargin);
    assert(randomSearch.initalDepthLMR == randomSearchClone.initalDepthLMR);
    assert(randomSearch.initialMoveCountLMR == randomSearchClone.initialMoveCountLMR);
    assert(randomSearch.killerMoveScore == randomSearchClone.killerMoveScore);
    assert(randomSearch.initialMoveCountLMR == randomSearchClone.initialMoveCountLMR);
    assert(randomSearch.useLazyEvalStatic == randomSearchClone.useLazyEvalStatic);
    assert(randomSearch.lmpMoveCount == randomSearchClone.lmpMoveCount);
    assert(randomSearch.nullMovePruningInitialReduction == randomSearchClone.nullMovePruningInitialReduction);
    assert(randomSearch.nullMovePruningDepthFactor == randomSearchClone.nullMovePruningDepthFactor);

    double initialMutationRate = 0.05;
    double decayRate = 0;
    double crossoverRate = 0.75;
    int totalGenerations = 200;
    size_t populationSize = 20; // because we have twenty threads, we will almost always have a pop size of at least 20. This corresponds to each thread playing 100 games per generation (still a lot less than 19X20 )
    int eliteSize = 2;
    int archiveSize = 1;
    int replacementCount = 400; // we can be more aggresive since we are deleting stuff after

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