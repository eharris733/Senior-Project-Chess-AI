// this ga is for tuning search parameters
// this class will be the main class for the genetic algorithm
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include "baselines.hpp"
#include "ga_util.hpp"
#include "searcher.hpp"
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <iostream>
#include <memory>


class GeneticAlgorithm4 {
public:
    GeneticAlgorithm4(size_t populationSize, double initialMutationRate, double mutationDecayRate, double crossoverRate, int totalGenerations, int trainingSize, int eliteCount, int archiveSize, int reintroduceCount, std::string trainingDataPath)
        : populationSize(populationSize), initialMutationRate(initialMutationRate), mutationDecayRate(mutationDecayRate),  crossoverRate(crossoverRate),totalGenerations(totalGenerations), trainingSize(trainingSize), eliteCount(eliteCount), archiveSize(archiveSize), reintroduceCount(reintroduceCount), trainingDataPath(trainingDataPath){
        readCSV(trainingDataPath); 
        selectNRandom(trainingSize); // Select 5,000 random evaluations
        initializePopulation();
        
        
    }

    void run() {

        Chromosome best = population.front();
        currentGeneration = 0;

        //try baseline
        std::cout << "Training Data Path" << trainingDataPath << std::endl;

        double baseline = calculateFitness(positions, baseSearch);
        std::cout << "Baseline Fitness: " << baseline << std::endl;

        while(currentGeneration++ < totalGenerations) {
            mutationRate = calculateMutationRate(initialMutationRate, mutationDecayRate, currentGeneration);
            
            selectNRandom(trainingSize); // Select x random evaluations
            crossover();
            mutation();

            // sort the population by fitness
            std::sort(population.begin(), population.end(), [](const Chromosome& a, const Chromosome& b) {
                return a.fitness > b.fitness;
            });

            // delete worst one and duplicate best one
            population.pop_back();
            population.push_back(population.front());
            
            if(population.front().fitness > best.fitness) {
                best = population.front();
            }

            // Example: Tracking best and average fitness
            double totalFitness = 0.0;

            

            best = population.front();
            TunableSearch bestSearch = convertChromosomeToSearch(best.data);
            double bestFitness = best.fitness; // Assuming population is sorted
            for (const auto& individual : population) {
                totalFitness += individual.fitness;
            }
            double averageFitness = totalFitness / population.size();
            std::cout << "Generation: " << currentGeneration << std::endl;
            cout << "Mutation Rate: " << mutationRate << endl;
            std::cout << "Best Fitness: " << bestFitness << ", Average Fitness: " << averageFitness << std::endl;

            if(currentGeneration % 5 == 0) {
                std::cout << "Best Chromosome: "  << std::endl;
                printTunableSearch(bestSearch);
            }
        }

        std::cout << "Best Chromosome: "  << std::endl;
        printTunableSearch(convertChromosomeToSearch(best.data));

    }


private:
    struct FenMovePair {
        std::string fen;
        std::string move;
    };

    struct Chromosome {
        std::string data; // This would represent your chromosome data structure
        double fitness;
    };

    // the parameters for the genetic algorithm, some set to default just in case
    size_t populationSize;
    double initialMutationRate;
    double mutationDecayRate;
    double mutationRate;
    double crossoverRate;
    int currentGeneration;
    int totalGenerations;
    int trainingSize;
    int eliteCount; // Number of best individuals to keep in each generation
    std::vector<Chromosome> archive; // for historical mechanism as described in 2013 paper
    size_t archiveSize; // N: Maximum size of the archive
    size_t reintroduceCount; // X: Number of solutions to reintroduce from the archive to each new generation


    std::vector<Chromosome> population; // a vector of pairs of chromosomes and their fitness levels
    std::vector<FenMovePair> allTrainingData;
    std::vector<FenMovePair> positions;
    std::random_device rd;
    std::mt19937 gen{rd()}; // random c++ magic stuff
    std::string trainingDataPath = "";

    

    void readCSV(std::string trainingDataPath) {
        Board board = Board();
        std::ifstream file(trainingDataPath);
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string fen, move;
            if (std::getline(ss, fen, ',') && std::getline(ss, move)) {
                // make sure its a valid fen
                try {
                    board.setFen(fen);
                    allTrainingData.push_back({fen, move});
                } catch (const std::exception& e) {
                    std::cerr << "An exception occurred while setting FEN: " << e.what() << std::endl;
                    std::cerr << "FEN: " << fen << std::endl;
                    std::cerr <<"line: " << line << std::endl;
                }
            }
        }
    }

    // simulated annealing, makes the mutation rate do exponential decay
    double calculateMutationRate(double initialRate, double decayRate, int generation) {
        return initialRate * exp(-decayRate * generation);
    }


double calculateFitnessSubset(const std::vector<FenMovePair>& posSubset, const TunableSearch& params, std::atomic<bool>& stopSignal) {
    double totalSuccessfulDepth = 0.0;
    Board board = Board();
    Searcher searcher = Searcher(board, params, baseEval, stopSignal); // Pass the stop signal here
    searcher.setVerbose(false);
    for (const FenMovePair& move : posSubset) {

        // Timer thread to set stopSignal after 1 second
        std::thread timer([&stopSignal]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            stopSignal.store(true);
        });
        searcher.clear();
        board.setFen(move.fen);
        SearchState result = searcher.search(200, 0, 1);
        Move bestMove = result.bestMove;
        int depthReached = result.currentDepth;
        std::string uciMove = uci::moveToUci(bestMove);
        
        if (timer.joinable()) {
            timer.join(); // Ensure the timer thread finishes
        }
        stopSignal.store(false); // Reset the stop signal for potential future use
        if (move.move == uciMove) {
            totalSuccessfulDepth += depthReached;
        }
    }
    return totalSuccessfulDepth;
}

double calculateFitness(const std::vector<FenMovePair>& pos, const TunableSearch& params) {
    const size_t numThreads = std::min(5u, std::thread::hardware_concurrency()); // Safeguard and optimization
    std::vector<std::thread> threads;
    std::vector<std::shared_ptr<std::atomic<bool>>> stopSignals;
    std::vector<double> partialDifferences(numThreads, 0.0); // To store the results from each thread

    size_t posPerThread = pos.size() / numThreads;

   auto worker = [&](size_t startIdx, size_t endIdx, size_t threadIdx, std::shared_ptr<std::atomic<bool>> stopSignal) {
    try {
        std::vector<FenMovePair> posSubset(pos.begin() + startIdx, pos.begin() + endIdx);
        partialDifferences[threadIdx] = calculateFitnessSubset(posSubset, params, *stopSignal);
    } catch (const std::exception& e) {
        std::cerr << "Exception in thread " << threadIdx << ": " << e.what() << std::endl;
    }
};

    for (size_t i = 0; i < numThreads; ++i) {
        size_t startIdx = i * posPerThread;
        size_t endIdx = (i + 1) * posPerThread;
        if (i == numThreads - 1) {
            endIdx = pos.size(); // Ensure the last thread covers the remainder
        }
            // Create a new stop signal for each search
        auto stopSignal = std::make_shared<std::atomic<bool>>(false);
        stopSignals.push_back(stopSignal);
        threads.emplace_back(std::thread(worker, startIdx, endIdx, i, stopSignals[i]));
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    double totalDifference = 0.0;
    for (const auto& diff : partialDifferences) {
        totalDifference += diff;
    }

    return totalDifference;
}


    // looks good
    void initializePopulation() {
        for (size_t i = 0; i < populationSize; ++i) {
            TunableSearch randomSearch = initializeRandomTunableSearch();
            population.push_back({convertSearchToChromosome(randomSearch), 0}); // 100000.0 is a placeholder for the fitness level
        }
    }

    void selectNRandom(size_t n) {
        positions.clear();
        std::uniform_int_distribution<> dist(0, allTrainingData.size() - 1);
        while (positions.size() < n) {
            positions.push_back(allTrainingData[dist(gen)]);
        }
        
    }

    // looks good
    // add history mechanism at some point
    void mutation() {
        for (auto& individual : population) {
            // find a random point in the chromosome to mutate
            // flip the bit at that point
            mutate(individual); // first is the chromosome
        }
    }

    // Function to perform single-point crossover between two chromosomes
    std::pair<std::string, std::string> singlePointCrossover(const std::string& parent1, const std::string& parent2) {
        // Ensure the parents are of equal length
        if (parent1.size() != parent2.size()) {
            throw std::invalid_argument("Parents must be of the same length for single-point crossover.");
        }

        // Randomly select the crossover point
        std::uniform_int_distribution<> distrib(1, parent1.size() - 2); // Range avoids trivial crossover points at the ends

        size_t crossoverPoint = distrib(gen);

        // Create offspring by combining the genetic material of the parents
        std::string offspring1 = parent1.substr(0, crossoverPoint) + parent2.substr(crossoverPoint);
        std::string offspring2 = parent2.substr(0, crossoverPoint) + parent1.substr(crossoverPoint);

        return {offspring1, offspring2};
    }


// alternative approach to selection based on the research paper
Chromosome rouletteWheelSelectionWithScaling(const std::vector<Chromosome>& parents) {
    // Find the maximum fitness in the population to scale fitness values
    double maxFitness = std::numeric_limits<double>::lowest();
    for (const auto& individual : parents) {
        if (individual.fitness > maxFitness) {
            maxFitness = individual.fitness;
        }
    }
    double scalingFactor = maxFitness + 1; // Add 1 or a small value to ensure even the best individual has a positive scaled fitness

    // Calculate the total scaled fitness of the population
    double totalScaledFitness = 0.0;
    for (const auto& individual : parents) {
        totalScaledFitness += (individual.fitness); // Invert the fitness
    }

    // Generate a random number between 0 and the total scaled fitness
    std::uniform_real_distribution<> dist(0.0, totalScaledFitness);
    double randomFitness = dist(gen);

    // Find and return the first individual for which the sum of scaled fitnesses up to that point exceeds the random number
    double currentSum = 0.0;
    for (const auto& individual : parents) {
        currentSum += (scalingFactor - individual.fitness); // Use scaled fitness
        if (currentSum >= randomFitness) {
            return individual;
        }
    }

    // This point should theoretically never be reached, but return the last individual as a fallback
    return population.back();
}

// random single point crossover
void crossover() {
    // Clear the population to prepare for the new generation
    std::vector<Chromosome> newPopulation;

    std::uniform_int_distribution<> parentDist(0, population.size() - 1); // Distribution for selecting parents

    while (newPopulation.size() < populationSize) {
        // select two parents based on the tournament selection
        const Chromosome& parent1 = rouletteWheelSelectionWithScaling(population);
        const Chromosome& parent2 = rouletteWheelSelectionWithScaling(population);

        // Check for crossover chance
        if (std::uniform_real_distribution<>(0.0, 1.0)(gen) < crossoverRate) {
            // Perform single-point crossover
            auto [child1, child2] = singlePointCrossover(parent1.data, parent2.data);

            // Calculate fitness for the new children and add them to the new population
            newPopulation.push_back({child1, calculateFitness(positions, convertChromosomeToSearch(child1))});
            if (newPopulation.size() < populationSize) { // Check to avoid exceeding population size
                newPopulation.push_back({child2, calculateFitness(positions, convertChromosomeToSearch(child2))});
            }
        } else {
            // If crossover does not occur, copy parents to the new population, checking not to exceed the population size
            if (newPopulation.size() < populationSize) {
                newPopulation.push_back(parent1);
            }
            if (newPopulation.size() < populationSize) {
                newPopulation.push_back(parent2);
            }
        }
    }

    // Update the original population with the new one
    population = std::move(newPopulation);
}

// Function to update the archive with solutions from the current generation
    void updateArchive(const std::vector<Chromosome>& currentGeneration) {
        // Combine current generation and archive, then sort
        std::vector<Chromosome> combined = archive;
        combined.insert(combined.end(), currentGeneration.begin(), currentGeneration.end());
        std::sort(combined.begin(), combined.end(), [](const Chromosome& a, const Chromosome& b) {
            return a.fitness > b.fitness; // Assuming higher fitness values are better
        });

        // Keep only the best N solutions in the archive
        if (combined.size() > archiveSize) {
            combined.resize(archiveSize);
        }

        archive = combined;
    }

    // Function to reintroduce X solutions from the archive to the new population
    void reintroduceFromArchive(std::vector<Chromosome>& newPopulation) {
        if (archive.empty() || reintroduceCount == 0) return;

        // Ensure we do not reintroduce more than available in the archive
        size_t count = std::min(reintroduceCount, archive.size());

        for (int i = 0; i < count; i++){
            Chromosome popped = archive.front();
            archive.pop_back();
            population.push_back(popped);
        }

        // Optionally, remove reintroduced solutions from the archive
        // archive.erase(archive.begin(), archive.begin() + count);
    }

// selection based on the research paper, inverted to make lower fitness better
// now with elitism
   void selection() {
    // Sort the population by fitness to identify the elites
    std::sort(population.begin(), population.end(), [](const Chromosome& a, const Chromosome& b) {
        return a.fitness > b.fitness; // Assuming higher fitness values are better
    });

    // Directly copy the eliteCount individuals to the new population
    std::vector<Chromosome> newPopulation(population.begin(), population.begin() + eliteCount);

    // Before filling the rest of the new population, reintroduce solutions from the archive
    reintroduceFromArchive(newPopulation);

    // Calculate the total inverted fitness of the non-elite population
    double totalInvertedFitness = 0.0;
    for (size_t i = eliteCount; i < population.size(); ++i) {
        double invertedFitness = population[i].fitness;
        totalInvertedFitness += invertedFitness;
    }

    // Calculate the selection probability for each non-elite individual
    std::vector<double> selectionProbabilities;
    for (size_t i = eliteCount; i < population.size(); ++i) {
        double invertedFitness = population[i].fitness;
        double selectionProbability = invertedFitness / totalInvertedFitness;
        selectionProbabilities.push_back(selectionProbability);
    }

    // Adjust the selection process for non-elites
    std::discrete_distribution<size_t> distribution(selectionProbabilities.begin(), selectionProbabilities.end());

    // Fill the remaining slots in the new population based on selection probabilities
    while (newPopulation.size() < populationSize) {
        size_t selectedIndex = distribution(gen) + eliteCount; // Adjust index to skip elites
        newPopulation.push_back(population[selectedIndex]);
    }

    // Before replacing the old population, update the archive with solutions from the current generation
        updateArchive(population);
    // Update the population with the new generation, including elites
    population = std::move(newPopulation);
}



    void mutate(Chromosome& c) {
        // Ensure the chromosome is not empty
        assert(c.data.size() > 0);
        // Create a uniform distribution for indices of the chromosome
        std::uniform_real_distribution<> dis(0.0, 1.0);

        for (size_t i = 0; i < c.data.size(); ++i) {
            if (dis(gen) < mutationRate) {
                // Flip the bit at the selected position
                c.data[i] = (c.data[i] == '0') ? '1' : '0';
                
            }
        }
        // recalculate the fitness level
        c.fitness = calculateFitness(positions, convertChromosomeToSearch(c.data));
        }

    };
