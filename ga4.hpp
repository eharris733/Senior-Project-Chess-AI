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
#include <iostream>
#include <memory>


class GeneticAlgorithm4 {
public:
    GeneticAlgorithm4(size_t populationSize, double initialMutationRate, double mutationDecayRate, double crossoverRate, int totalGenerations, int trainingSize, int eliteCount, int archiveSize, int reintroduceCount, std::string trainingDataPath)
        : populationSize(populationSize), initialMutationRate(initialMutationRate), mutationDecayRate(mutationDecayRate),  crossoverRate(crossoverRate),totalGenerations(totalGenerations), trainingSize(trainingSize), eliteCount(eliteCount), archiveSize(archiveSize), reintroduceCount(reintroduceCount), trainingDataPath(trainingDataPath){
        readCSV(trainingDataPath); 
        selectNRandom(trainingSize); // Select 5,000 random evaluations
        initializePopulation();
        calculateFitness();
    }

    void run() {

        Chromosome best = population.front();
        currentGeneration = 0;

        //try baseline
        std::cout << "Training Data Path" << trainingDataPath << std::endl;

        // double baseline = calculateFitness();
        // std::cout << "Baseline Fitness: " << baseline << std::endl;
        selectNRandom(trainingSize); // Select 5,000 random evaluations
        assert(population.size() == populationSize);
        assert(positions.size() == trainingSize);

        while(currentGeneration++ < totalGenerations) {
            mutationRate = calculateMutationRate(initialMutationRate, mutationDecayRate, currentGeneration);
            
            selectNRandom(trainingSize); // Select x random evaluations
            crossover();
            mutation();
            calculateFitness();
            assert(population.size() == populationSize);
            // sort the population by fitness
            std::sort(population.begin(), population.end(), [](const Chromosome& a, const Chromosome& b) {
                return a.fitness > b.fitness;
            });
            
            // we only care about the best one in THIS generation
            best = population.front();

            // Example: Tracking best and average fitness
            double totalFitness = 0.0;
            // print out every generation to track learning
            TunableSearch bestSearch = convertChromosomeToSearch(best.data);
            double bestFitness = best.fitness; // Assuming population is sorted
            for (const auto& individual : population) {
                totalFitness += individual.fitness;
            }
            double averageFitness = totalFitness / population.size();
            std::cout << "Generation: " << currentGeneration << std::endl;
            cout << "Mutation Rate: " << mutationRate << endl;
            std::cout << "Best Fitness: " << bestFitness << ", Average Fitness: " << averageFitness << std::endl;

            //print out every generation to track learning
            std::cout << "Best Chromosome: "  << std::endl;
            printTunableSearch(bestSearch);
        }

        std::cout << "Best Chromosome: "  << std::endl;
        printTunableSearch(convertChromosomeToSearch(best.data));

    }


private:
    
    struct FenMovePair {
        std::string fen = chess::constants::STARTPOS;
        std::string move = "";
    };

    struct Chromosome {
        std::string data = ""; // This would represent your chromosome data structure
        double fitness = 0.0;
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


    std::vector<Chromosome> population = vector<Chromosome>(); // a vector of pairs of chromosomes and their fitness levels
    std::vector<FenMovePair> allTrainingData = vector<FenMovePair>();
    std::vector<FenMovePair> positions = vector<FenMovePair>();
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


    double calculateFitnessSubset(const std::vector<FenMovePair>& posSubset, const TunableSearch& params) {
        double totalSuccessfulDepth = 0.0;
        thread_local Board board;
        thread_local Searcher searcher = Searcher(board, params, ga1result10); // Use fully qualified name 

        for (const FenMovePair& move : posSubset) {
            board.setFen(move.fen);
            searcher.clear();
            searcher.setVerbose(false);
            SearchState result = searcher.search(60, 0, 1); // since we have the ten second overhead
            Move bestMove = result.bestMove;
            int depthReached = result.currentDepth;
            std::string uciMove = uci::moveToUci(bestMove);
            if (move.move == uciMove) {
                totalSuccessfulDepth += 1;
            }
        }

        return totalSuccessfulDepth;
    }



double calculateFitness() {
    std::mutex updateMutex;
    std::vector<std::thread> threads;
    std::vector<double> fitnessScores(populationSize, 0.0); // Prepare a vector to store fitness scores

    // Iterate over each chromosome in the population to create a thread for each
    for (size_t i = 0; i < populationSize; ++i) {
        threads.emplace_back([&, i]() {
            // Convert the chromosome to TunableSearch parameters for this thread's chromosome
            TunableSearch params = convertChromosomeToSearch(population[i].data);

            // Calculate fitness over the entire subset of positions for this chromosome
            double fitness = calculateFitnessSubset(positions, params);

            // Safely update the fitness score for this chromosome
            std::lock_guard<std::mutex> guard(updateMutex);
            fitnessScores[i] = fitness;
        });
    }

    // Wait for all threads to complete their execution
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    // Update the population with the new fitness scores
    for (size_t i = 0; i < populationSize; ++i) {
        population[i].fitness = fitnessScores[i];
    }

    // Optionally, calculate and return the average fitness for logging or other purposes
    double totalFitness = std::accumulate(fitnessScores.begin(), fitnessScores.end(), 0.0);
    return totalFitness / static_cast<double>(populationSize);
}



    // looks good
    void initializePopulation() {
        for (size_t i = 0; i < populationSize; ++i) {
            thread_local TunableSearch randomSearch = initializeRandomTunableSearch();
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


Chromosome rouletteWheelSelectionWithScaling(const std::vector<Chromosome>& parents) {
    // Calculate the total fitness of the population
    double totalFitness = 0.0;
    for (const auto& individual : parents) {
        totalFitness += individual.fitness;
    }

    // Generate a random number between 0 and the total fitness
    std::uniform_real_distribution<> dist(0.0, totalFitness);
    double randomFitness = dist(gen);

    // Accumulate fitness and select the first individual whose accumulated fitness exceeds the random number
    double currentSum = 0.0;
    for (const auto& individual : parents) {
        currentSum += individual.fitness;
        if (currentSum >= randomFitness) {
            return individual;
        }
    }

    // Ideally, the loop should always return an individual. This line is a safety net.
    return parents.back();
}


// random single point crossover
void crossover() {
    // Clear the population to prepare for the new generation
    std::vector<Chromosome> newPopulation;

    std::uniform_int_distribution<> parentDist(0, population.size() - 1); // Distribution for selecting parents

    while (newPopulation.size() < populationSize) {
        // select two parents based on the proportional selection
        const Chromosome& parent1 = rouletteWheelSelectionWithScaling(population);
        const Chromosome& parent2 = rouletteWheelSelectionWithScaling(population);

        // Check for crossover chance
        if (std::uniform_real_distribution<>(0.0, 1.0)(gen) < crossoverRate) {
            // Perform single-point crossover
            auto [child1, child2] = singlePointCrossover(parent1.data, parent2.data);

            // Calculate fitness for the new children and add them to the new population
            newPopulation.push_back({child1, 0});
            if (newPopulation.size() < populationSize) { // Check to avoid exceeding population size
                newPopulation.push_back({child2, 0});
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

    // Directly use fitness for calculating selection probabilities
    double totalFitness = 0.0;
    for (size_t i = eliteCount; i < population.size(); ++i) {
        totalFitness += population[i].fitness;
    }

    std::vector<double> selectionProbabilities;
    for (size_t i = eliteCount; i < population.size(); ++i) {
        double selectionProbability = population[i].fitness / totalFitness;
        selectionProbabilities.push_back(selectionProbability);
    }

    std::discrete_distribution<size_t> distribution(selectionProbabilities.begin(), selectionProbabilities.end());

    // Now, simply fill the new population slots based on the computed probabilities
    while (newPopulation.size() < populationSize) {
        size_t selectedIndex = distribution(gen) + eliteCount;
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
        c.fitness = 0;
        }

    };
