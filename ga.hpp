// this class will be the main class for the genetic algorithm
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include "baselines.hpp"
#include "ga_util.hpp"
#include "searcher.hpp"
#include <atomic>


atomic<bool> stop(false); // so the searcher works

class GeneticAlgorithm {
public:
    GeneticAlgorithm(size_t populationSize, double mutationRate,  double crossoverRate, int totalGenerations, int trainingSize)
        : populationSize(populationSize), mutationRate(mutationRate), crossoverRate(crossoverRate),totalGenerations(totalGenerations), trainingSize(trainingSize) {
        readCSV("dbs/fen_cp_evaluations.csv"); 
        selectNRandom(trainingSize); // Select 5,000 random evaluations
        initializePopulation();
        
        
    }

    void run() {
        currentGeneration = 0;

        while(currentGeneration++ < totalGenerations) {
            selectNRandom(trainingSize); // Select 5,000 random evaluations
            crossover();
            mutation();

            // sort the population by fitness
            std::sort(population.begin(), population.end(), [](const chromosome& a, const chromosome& b) {
                return a.fitness < b.fitness;
            });

            // delete worst one and duplicate best one
            population.pop_back();
            population.push_back(population.front());

            // Example: Tracking best and average fitness
            double totalFitness = 0.0;

            

            chromosome best = population.front();
            TunableEval bestEval = convertChromosoneToEval(best.chromosome);
            double bestFitness = best.fitness; // Assuming population is sorted
            for (const auto& individual : population) {
                totalFitness += individual.fitness;
            }
            double averageFitness = totalFitness / population.size();
            std::cout << "Generation: " << currentGeneration << std::endl;
            std::cout << "Best Fitness: " << bestFitness << ", Average Fitness: " << averageFitness << std::endl;

            if(currentGeneration % 20 == 0) {
                std::cout << "Best Chromosome: "  << std::endl;
                printTunableEval(bestEval);
            }
        }

    }


private:
    // for reading the csv file
    struct PositionEvaluation {
        std::string fen;
        double actualScore;
    };

    struct chromosome {
        std::string chromosome;
        double fitness;
    };

    // the parameters for the genetic algorithm, some set to default just in case
    size_t populationSize;
    double mutationRate;
    double crossoverRate;
    int currentGeneration;
    int totalGenerations;
    int trainingSize;
    
    std::vector<chromosome> population; // a vector of pairs of chromosomes and their fitness levels
    std::vector<PositionEvaluation> allEvaluations;
    std::vector<PositionEvaluation> evaluations;
    std::random_device rd;
    std::mt19937 gen{rd()}; // random c++ magic stuff

    

    void readCSV(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string fen;
            double score;
            if (std::getline(ss, fen, ',') && ss >> score) {
                allEvaluations.push_back({fen, score});
            }
        }
    }  

    double calculateFitness(const std::vector<PositionEvaluation>& evals, const TunableEval& params) {

        Board board = Board();
        Searcher searcher(board, baseSearch, params); // Create a new Searcher object with default 
        //feature weights
        searcher.setMaxDepth(1); // only do 1 -ply searches for now
        searcher.setVerbose(false); // Turn off verbose mode
        double totalDifference = 0.0;

        for (const auto& eval : evals) {
            board.setFen(eval.fen);
            searcher.clear(); // Clear the TT table bc new position has nothing to do w old one
            double predictedScore = searcher.search(1000, 0, 1).bestScore; // kind of irrelevant params here
            // if(predictedScore == 0) {
            //     cout << "predicted score is 0" << endl;
            // }
            totalDifference += std::abs(predictedScore - eval.actualScore);
        }

        return totalDifference / evaluations.size(); // Return the average difference
    }

    // looks good
    void initializePopulation() {
        for (size_t i = 0; i < populationSize; ++i) {
            TunableEval randomEval = initializeRandomTunableEval();
            population.push_back({convertEvalToChromosone(randomEval), calculateFitness(evaluations, randomEval)}); // 100000.0 is a placeholder for the fitness level
        }
    }

    void selectNRandom(size_t n) {
        evaluations.clear();
        std::uniform_int_distribution<> dist(0, allEvaluations.size() - 1);
        while (evaluations.size() < n) {
            evaluations.push_back(allEvaluations[dist(gen)]);
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

    // not good yet
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

    // Tournament selection function
chromosome tournamentSelection(const std::vector<chromosome>& parents, int tournamentSize) {
    std::uniform_int_distribution<> dist(0, parents.size() - 1);
    int bestIndex = dist(gen); // Randomly select one individual as the initial "best"
    double bestFitness = parents[bestIndex].fitness;

    // Perform the tournament
    for (int i = 1; i < tournamentSize; ++i) {
        int newIndex = dist(gen); // Select another individual
        if (parents[newIndex].fitness < bestFitness) { // Assuming lower fitness is better
            bestFitness = parents[newIndex].fitness;
            bestIndex = newIndex;
        }
    }

    return population[bestIndex]; // Return the best individual from the tournament
}

// alternative approach to selection based on the research paper
chromosome rouletteWheelSelectionWithScaling(const std::vector<chromosome>& parents) {
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
        totalScaledFitness += (scalingFactor - individual.fitness); // Invert the fitness
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
    std::vector<chromosome> newPopulation;

    std::uniform_int_distribution<> parentDist(0, population.size() - 1); // Distribution for selecting parents

    while (newPopulation.size() < populationSize) {
        // select two parents based on the tournament selection
        const chromosome& parent1 = rouletteWheelSelectionWithScaling(population);
        const chromosome& parent2 = rouletteWheelSelectionWithScaling(population);

        // Check for crossover chance
        if (std::uniform_real_distribution<>(0.0, 1.0)(gen) < crossoverRate) {
            // Perform single-point crossover
            auto [child1, child2] = singlePointCrossover(parent1.chromosome, parent2.chromosome);

            // Calculate fitness for the new children and add them to the new population
            newPopulation.push_back({child1, calculateFitness(evaluations, convertChromosoneToEval(child1))});
            if (newPopulation.size() < populationSize) { // Check to avoid exceeding population size
                newPopulation.push_back({child2, calculateFitness(evaluations, convertChromosoneToEval(child2))});
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

// selection based on the research paper, inverted to make lower fitness better
   void selection() {
    // Find the maximum fitness in the population to scale fitness values
    double maxFitness = std::max_element(population.begin(), population.end(), 
                                          [](const chromosome& a, const chromosome& b) {
                                              return a.fitness < b.fitness;
                                          })->fitness;

    // Calculate the total inverted fitness of the current population
    double totalInvertedFitness = 0.0;
    for (const auto& individual : population) {
        double invertedFitness = maxFitness - individual.fitness; // Invert the fitness score
        totalInvertedFitness += invertedFitness;
    }

    // Calculate the selection probability for each individual based on inverted fitness
    std::vector<double> selectionProbabilities;
    for (const auto& individual : population) {
        double invertedFitness = maxFitness - individual.fitness;
        double selectionProbability = invertedFitness / totalInvertedFitness;
        selectionProbabilities.push_back(selectionProbability);
    }

    // Generate a new population based on selection probabilities
    std::vector<chromosome> newPopulation;
    std::discrete_distribution<size_t> distribution(selectionProbabilities.begin(), selectionProbabilities.end());

    // Fill the new population based on selection probabilities
    for (size_t i = 0; i < populationSize; i++) {
        size_t selectedIndex = distribution(gen);
        newPopulation.push_back(population[selectedIndex]);
    }

    // Update the population with the new generation
    population = std::move(newPopulation);

}



    void mutate(chromosome& c) {
        // Ensure the chromosome is not empty
        assert(c.chromosome.size() > 0);
        // Create a uniform distribution for indices of the chromosome
        std::uniform_real_distribution<> dis(0.0, 1.0);

        for (size_t i = 0; i < c.chromosome.size(); ++i) {
            if (dis(gen) < mutationRate) {
                // Flip the bit at the selected position
                c.chromosome[i] = (c.chromosome[i] == '0') ? '1' : '0';
                
            }
        }
        // recalculate the fitness level
        c.fitness = calculateFitness(evaluations, convertChromosoneToEval(c.chromosome));
        }

    };
