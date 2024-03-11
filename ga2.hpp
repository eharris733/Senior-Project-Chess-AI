#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include "baselines.hpp"
#include "ga_util.hpp"
#include "searcher.hpp"
#include <iostream>
#include "chess.hpp"


std::atomic<bool> stop(false);

class GA2 {
public:
    GA2(std::string trainingDataPath, size_t populationSize, double mutationThreshold, double mutationRate, int totalGenerations, int trainingSize)
    : trainingDataPath(trainingDataPath), populationSize(populationSize), mutationThreshold(mutationThreshold), mutationRate(mutationRate), totalGenerations(totalGenerations), trainingSize(trainingSize) {
        readCSV();
        selectNRandom();
        initializePopulation();
    }

    void run() {
        double baseline = calculateFitness(baseEval);
        std::cout << "Baseline Fitness: " << baseline << std::endl;
        // double zeroBaseline = calculateFitness(zeroEval);
        // std::cout << "Zero Fitness: " << zeroBaseline << std::endl;

        for (int generation = 0; generation < totalGenerations; ++generation) {
            selectNRandom();
            selection();
            updateArchive();
            mutation();

            // sort the population by fitness
            std::sort(population.begin(), population.end(), [](const Chromosome& a, const Chromosome& b) {
                return a.fitness > b.fitness;
            });

            double totalFitness = 0.0;
            Chromosome best = population.front();
            TunableEval bestEval = convertChromosoneToEval(best.data);
            double bestFitness = best.fitness; // Assuming population is sorted
            for (const auto& individual : population) {
                totalFitness += individual.fitness;
            }
            double averageFitness = totalFitness / population.size();
            std::cout << "Generation: " << generation << std::endl;
            cout << "Mutation Rate: " << mutationRate << endl;
            std::cout << "Best Fitness: " << bestFitness << ", Average Fitness: " << averageFitness << std::endl;

            if(generation % 20 == 0) {
                std::cout << "Best Chromosome: "  << std::endl;
                printTunableEval(bestEval);
            }

            assert(populationSize == population.size());


        }
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

    std::vector<FenMovePair> allTrainingData;
    std::vector<FenMovePair> trainingData;
    std::vector<Chromosome> population;
    std::string trainingDataPath;
    size_t populationSize;
    int trainingSize;
    double mutationThreshold; // whether or not we introduce from archive
    double mutationRate; // chance we flip a bit during mutation
    int totalGenerations;
    std::mt19937 gen{std::random_device{}()};
    std::vector<Chromosome> archive;

    void readCSV() {
        Board board;
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

    void selectNRandom() {
        trainingData.clear();
        std::uniform_int_distribution<> dist(0, allTrainingData.size() - 1);
        while (trainingData.size() < trainingSize) {
            trainingData.push_back(allTrainingData[dist(gen)]);
        }
    }

    void initializePopulation() {
        // Implement your method for initializing the population
       for (size_t i = 0; i < populationSize; ++i) {
            TunableEval randomEval = initializeRandomTunableEval();
            population.push_back({convertEvalToChromosone(randomEval), calculateFitness(randomEval)}); // 100000.0 is a placeholder for the fitness level
        }
    }

    double calculateFitnessSubset(const std::vector<FenMovePair>& evalsSubset, const TunableEval& params) {
        int numCorrect = 0;
        for (const auto& eval : evalsSubset) {
            Board board = Board();
            Searcher searcher = Searcher(board, baseSearch, params, 2 << 16); // useless size for a tt
            searcher.setMaxDepth(1);
            searcher.setVerbose(false);
            board.setFen(eval.fen);
            Move predictedMove = searcher.search(1000, 0, 1).bestMove;
            searcher.clear();
            if (predictedMove == uci::uciToMove(board, eval.move)){
                numCorrect++;
            }
            
        }
        double percentage = (double)numCorrect / evalsSubset.size();
        return percentage;
    }

   double calculateFitness(const TunableEval& params) {
    const size_t numThreads = std::thread::hardware_concurrency(); // Use as many threads as there are CPU cores - 1
    std::vector<std::thread> threads;
    std::vector<double> partialDifferences(numThreads, 0.0); // To store the results from each thread

    size_t evalsPerThread = trainingData.size() / numThreads;
    
    // Lambda to process each subset of evaluations
   auto worker = [&](size_t startIdx, size_t endIdx, size_t threadIdx) {
    try {
        std::vector<FenMovePair> evalsSubset(trainingData.begin() + startIdx, trainingData.begin() + endIdx);
        partialDifferences[threadIdx] = calculateFitnessSubset(evalsSubset, params);
    } catch (const std::exception& e) {
        std::cerr << "Exception in thread " << threadIdx << ": " << e.what() << std::endl;
        // Handle exception or set an error state
    }
};


    // Launch threads
    for (size_t i = 0; i < numThreads; ++i) {
        size_t startIdx = i * evalsPerThread;
        size_t endIdx = (i + 1) * evalsPerThread;
        if (i == numThreads - 1) {
            endIdx = trainingData.size(); // Make sure the last thread covers all remaining evaluations
        }
        threads.emplace_back(worker, startIdx, endIdx, i);
    }

    // Wait for all threads to complete
    for (auto& t : threads) {
        t.join();
    }

    // Aggregate the results
    double totalDifference = 0.0;
    for (const auto& diff : partialDifferences) {
        totalDifference += diff;
    }

    return totalDifference / partialDifferences.size(); // Return the average difference
}

    // essentially sort the population and take the top half
    void selection() {
        // Implement your method for selection
        // Sort the population based on fitness and take the top half
        for (size_t i = 0; i < population.size(); ++i) {
            population[i].fitness = calculateFitness(convertChromosoneToEval(population[i].data));
        }
        std::sort(population.begin(), population.end(), [](const Chromosome& a, const Chromosome& b) {
            return a.fitness > b.fitness;
        });
        population.resize(populationSize / 2);
    }

    void mutation() {
        int reintroduce = 0;
        for (auto& chromosome : population) {
            if (std::generate_canonical<double, 10>(gen) < mutationThreshold) {
                Chromosome mutated = chromosome;
                mutate(mutated);
                mutated.fitness = calculateFitness(convertChromosoneToEval(mutated.data));
                population.push_back(mutated);
            }
            else{
                reintroduce++;
            }
        }
        for (int i = 0; i < reintroduce; i++){
            reintroduceFromArchive();
        }
    }

    void mutate(Chromosome& c) {
            // Ensure the chromosome is not empty
            assert(c.data.size() > 0);
            // Create a uniform distribution for indices of the chromosome
            std::uniform_real_distribution<> dis(0.0, 1.0);

            for (size_t i = 0; i < c.data.size(); ++i) {
                if (dis(gen) < mutationThreshold) {
                    // Flip the bit at the selected position
                    c.data[i] = (c.data[i] == '0') ? '1' : '0';
                    
                }
            }
        }


    // Function to update the archive with solutions from the current generation
    void updateArchive() {
        // Combine current generation and archive, then sort
        std::vector<Chromosome> combined = archive;
        combined.insert(combined.end(), population.begin(), population.end());
        std::sort(combined.begin(), combined.end(), [](const Chromosome& a, const Chromosome& b) {
            return a.fitness > b.fitness; // Assuming lower fitness values are better
        });

        // Keep only the best 100 solutions in the archive
        if (combined.size() > 100) {
            combined.resize(100);
        }

        archive = combined;
    }

    // Function to reintroduce 1 solution from the archive to the new population
    void reintroduceFromArchive() {
        if (archive.empty()) return;

        population.push_back(archive.front());
    }
};


