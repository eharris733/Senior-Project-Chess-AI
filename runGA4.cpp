#include "ga_util.hpp"
#include "ga4.hpp"
#include "chess.hpp"
#include <string>
#include <cassert> // Add missing include for assert

// for now just going to test my utility functions
int main() {
    TunableSearch randomSearch = initializeRandomTunableSearch();
    std::string randomChromosone = convertSearchToChromosome(randomSearch);
    TunableSearch randomSearchClone = convertChromosomeToSearch(randomChromosone);

    // assert to see if they are the same
    assert(randomSearch.aspirationWindow1 == randomSearchClone.aspirationWindow1); 
    assert(randomSearch.aspirationWindow2 == randomSearchClone.aspirationWindow2);
    assert(randomSearch.aspirationWindowInitialDelta == randomSearchClone.aspirationWindowInitialDelta);
    assert(randomSearch.useAspirationWindowDepth == randomSearchClone.useAspirationWindowDepth);
    assert(randomSearch.deltaMargin == randomSearchClone.deltaMargin);
    assert(randomSearch.futilityMargin1 == randomSearchClone.futilityMargin1);
    assert(randomSearch.futilityMargin2 == randomSearchClone.futilityMargin2);
    assert(randomSearch.futilityMargin3 == randomSearchClone.futilityMargin3);
    assert(randomSearch.initalDepthLMR == randomSearchClone.initalDepthLMR);
    assert(randomSearch.initialMoveCountLMR == randomSearchClone.initialMoveCountLMR);
    assert(randomSearch.killerMoveScore == randomSearchClone.killerMoveScore);
    assert(randomSearch.promotionMoveScore == randomSearchClone.promotionMoveScore);
    assert(randomSearch.initialMoveCountLMR == randomSearchClone.initialMoveCountLMR);
    assert(randomSearch.useLazyEvalStatic == randomSearchClone.useLazyEvalStatic);

    

    // values from 2008 paper
    // population size = 1000
    // crossover rate = 0.75
    // mutation rate = 0.002
    // number of generations = 300
    // vars to time the algorithm
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();



    double initialMutationRate = 0.05;
    double decayRate = 0.015; // results in a final mutation of .000125
    double crossoverRate = 0.75;
    int totalGenerations = 200;
    size_t populationSize = 20; // always going to be 20
    int trainingSize = 952; // every single position
    int eliteSize = 1;
    int archiveSize = 400;
    int replacementCount = 1;
    std::string trainingDataPath = "dbs/test_suite.csv";

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

    
    GeneticAlgorithm4 GA = GeneticAlgorithm4(populationSize, initialMutationRate, decayRate, crossoverRate, totalGenerations, trainingSize, eliteSize, archiveSize, replacementCount, trainingDataPath);
    GA.run();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " seconds" << std::endl;
    return 0;
}
