// this class will be the main class for the genetic algorithm
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include "baselines.hpp"
#include "ga_util.hpp"
#include "searcher.hpp"
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>
#include <numeric>
#include <iostream>
#include <cassert>
#include "logger.hpp"



class GA5 {
public:
    GA5(size_t populationSize, double initialMutationRate, double mutationDecayRate, double crossoverRate, int totalGenerations, int eliteCount, int archiveSize, int reintroduceCount)
        : populationSize(populationSize), initialMutationRate(initialMutationRate), mutationDecayRate(mutationDecayRate),  crossoverRate(crossoverRate),totalGenerations(totalGenerations), eliteCount(eliteCount), archiveSize(archiveSize), reintroduceCount(reintroduceCount), book("Titans.bin") {
        initializePopulation();
        
    }

    void run() {

        chromosome best = population.front();
        chromosome worst = population.front();
        currentGeneration = 0;

        while(currentGeneration++ < totalGenerations) {

            mutationRate = calculateMutationRate(initialMutationRate, mutationDecayRate, currentGeneration);
            selection();
            crossover();
            mutation();
            calculateFitness();
            // Before replacing the old population, update the archive with solutions from the current generation
            updateArchive(population);

            // sort the population by fitness
            std::sort(population.begin(), population.end(), [](const chromosome& a, const chromosome& b) {
                return a.fitness > b.fitness;
            });

            
            if(population.front().fitness < best.fitness) {
                best = population.front();
            }
            if(population.back().fitness > worst.fitness) {
                worst = population.back();
            }

            // Example: Tracking best and average fitness
            double totalFitness = 0.0;

            

            chromosome best = population.front();
            chromosome worst = population.back();
            TunableSearch bestSearch = convertChromosomeToSearch(best.chromosome);
            double bestFitness = best.fitness; // Assuming population is sorted
            double worstFitness = worst.fitness; // Assuming population is sorted
            for (const auto& individual : population) {
                totalFitness += individual.fitness;
            }
            double averageFitness = totalFitness / population.size();
            Logger::getInstance().log( "Generation: " + std::to_string( currentGeneration));
            Logger::getInstance().log( "Mutation Rate: " + to_string(mutationRate)); 
            Logger::getInstance().log( "Best Fitness: " + to_string(bestFitness) + ", Average Fitness: " + to_string(averageFitness) + ", Worst Fitness: " + to_string(worstFitness)); 

            if(currentGeneration % 10 == 0) {
                Logger::getInstance().log("Best Chromosome: "); 
                printTunableSearch(bestSearch);
            }
            
            
        }

        std::cout << "Best Chromosome from archive: "  << std::endl;
        printTunableSearch(convertChromosomeToSearch(archive.front().chromosome));

    }


private:

    struct chromosome {
        std::string chromosome;
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
    int eliteCount; // Number of best individuals to keep in each generation
    std::vector<chromosome> archive; // for historical mechanism as described in 2013 paper
    size_t archiveSize; // N: Maximum size of the archive
    size_t reintroduceCount; // X: Number of solutions to reintroduce from the archive to each new generation
    PolyglotBook book;

    std::vector<chromosome> population; // a vector of pairs of chromosomes and their fitness levels
    std::random_device rd;
    std::mt19937 gen{rd()}; // random c++ magic stuff



    // simulated annealing, makes the mutation rate do exponential decay
    double calculateMutationRate(double initialRate, double decayRate, int generation) {
        return initialRate * exp(-decayRate * generation);
    }


    // Function to simulate a game between two searchers
int simulateGame(Searcher& whiteSearcher, Searcher& blackSearcher, Board& board) {
    int result = 0;
    int moveCount = 0;
    board.setFen(constants::STARTPOS); // Set the board to the starting position
    // play 8 moves into the book, randomly selecting moves
    Move openingMove = book.pickRandomMove(board);
    while (openingMove != Move::NULL_MOVE && moveCount < 8) {
        if (openingMove == Move::NULL_MOVE) {
            std::cerr << "Error: " << "Null Move in opening" << std::endl;
            std::cerr << "Move: " << openingMove << std::endl;
            std::cerr << "Fen: " << board.getFen() << std::endl; 
            break;
        }
        board.makeMove(openingMove);
        moveCount++;
        openingMove = book.pickRandomMove(board);
    }
    whiteSearcher.setVerbose(false); // Disable verbose output for white
    blackSearcher.setVerbose(false); // Disable verbose output for black
    // cap games at 100 moves for time
    while (board.isGameOver().second == GameResult::NONE) {
        SearchState searchResult;

        if (board.sideToMove() == Color::WHITE) {
            searchResult = whiteSearcher.search(60, 0, 1); //1000ms, 0ms increment, 1 move to go
        } else {
            searchResult = blackSearcher.search(60, 0, 1); // should reach max depth first, but just in case, call cutoff after one fourth of a second in bad circumstances
        }
        if (searchResult.bestScore < -500){
            if (board.sideToMove() == Color::WHITE){
                result = -1;
            }
            else{
                result = 1;
            }
            break;
        }


            if (searchResult.bestMove == Move::NULL_MOVE) {
                std::cerr << "Move: " << searchResult.bestMove << std::endl;
                std::cerr << "Fen: " << board.getFen() << std::endl; 
                result = 0; // inconclusive result, might skew things
                // but happens so rarely that its probably fine
                Logger::getInstance().log("Error:  Null Move returned from search, game aborted");
                break;   
            }
            board.makeMove(searchResult.bestMove); // Assume makeMove applies the move to the board
            moveCount++;
        
        // if the game is 100 moves for each side, too long, draw
        if (moveCount > 200) {
            result = 0;
            break;
        } 
    }
    // reach this point GameReslult is not NONE, so must be DRAW, WIN, or LOSE
    // Check for game end conditions    
        if (board.isGameOver().second == GameResult::LOSE) {
            result = board.sideToMove() == Color::WHITE ? -1: 1;
        } else if (board.isGameOver().second == GameResult::DRAW){
            result = 0;
        }
        else if (board.isGameOver().second == GameResult::WIN){
            result = board.sideToMove() == Color::WHITE ? 1: -1;
        }
    return result;
}

    // Example of TunableEval to Searcher conversion not shown, assuming direct use
double calculateFitnessSingleGame(const TunableSearch& params, const TunableSearch& opponent) {
    double totalPoints = 0.0;
    Board board = Board();
    Searcher white = Searcher(board, params, ga1result10); 

    
        Searcher black = Searcher(board, opponent, ga1result10);
        return simulateGame(white, black, board);
}






double calculateFitness() {
    std::vector<std::thread> threads;
    std::vector<double> fitnessScores(populationSize, 0.0); // Initial fitness scores
    std::mutex updateMutex;

    // Launch a thread for each player in the population
    for (int i = 0; i < populationSize; i++) {
        threads.emplace_back([&](int playerIndex) {
            double playerScore = 0.0; // Player's total score

            // Iterate through all opponents
            for (int j = 0; j < populationSize; j++) {
                if (j != playerIndex) { // Skip playing against oneself
                    TunableSearch opponentSearch = convertChromosomeToSearch(population[j].chromosome);
                    // Assuming calculateFitnessForSingleMatch handles a match between two players
                    double matchResult = calculateFitnessSingleGame(convertChromosomeToSearch(population[playerIndex].chromosome), opponentSearch);

                    std::lock_guard<std::mutex> guard(updateMutex);
                    playerScore += matchResult; // Update player's score
                    // Update opponent's score based on game result, consider draw and loss
                    fitnessScores[j] += (matchResult * -1); 
                }
            }

            // After all matches for this player are done, update their total fitness score
            {
                std::lock_guard<std::mutex> guard(updateMutex);
                fitnessScores[playerIndex] = playerScore;
            }
        }, i); // Pass the current player index to the thread
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    for (size_t i = 0; i < populationSize; ++i) {
        population[i].fitness = fitnessScores[i];
    }

    // Optionally, aggregate or further process fitnessScores as needed
    double totalFitness = std::accumulate(fitnessScores.begin(), fitnessScores.end(), 0.0);
    return totalFitness / static_cast<double>(populationSize); // Average fitness, adjust as needed
}





   

    // uses the ten dad values, reproduced population / 10 times
    void initializePopulation() {
        for (size_t i = 0; i < populationSize; ++i) {
            std::string chromosome = convertSearchToChromosome(initializeRandomTunableSearch());
            population.push_back({chromosome, 0});
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


chromosome rouletteWheelSelectionWithScaling(const std::vector<chromosome>& parents) {
    // Find the minimum fitness value to adjust negative fitness values
    double minFitness = std::numeric_limits<double>::max();
    for (const auto& individual : parents) {
        if (individual.fitness < minFitness) {
            minFitness = individual.fitness;
        }
    }
    double fitnessOffset = (minFitness < 0) ? -minFitness : 0;

    // Calculate the total adjusted fitness of the population
    double totalFitness = 0.0;
    for (const auto& individual : parents) {
        totalFitness += individual.fitness + fitnessOffset;
    }

    std::uniform_real_distribution<> dist(0.0, totalFitness);
    double randomFitness = dist(gen);

    double currentSum = 0.0;
    for (const auto& individual : parents) {
        currentSum += individual.fitness + fitnessOffset;
        if (currentSum >= randomFitness) {
            return individual;
        }
    }

    // This point should theoretically never be reached, but return the last individual as a fallback
    return parents.back();
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
            newPopulation.push_back({child1, 0});
            if (newPopulation.size() < populationSize) { // Check to avoid exceeding population size
                newPopulation.push_back({child2, 0}); // initialize at 0
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
    void updateArchive(const std::vector<chromosome>& currentGeneration) {
        // Combine current generation and archive, then sort
        std::vector<chromosome> combined = archive;
        combined.insert(combined.end(), currentGeneration.begin(), currentGeneration.end());
        std::sort(combined.begin(), combined.end(), [](const chromosome& a, const chromosome& b) {
            return a.fitness > b.fitness; // Assuming lower fitness values are better
        });

        // Keep only the best N solutions in the archive
        if (combined.size() > archiveSize) {
            combined.resize(archiveSize);
        }

        archive = combined;
    }

    // Function to reintroduce X solutions from the archive to the new population
    void reintroduceFromArchive(std::vector<chromosome>& newPopulation) {
        if (archive.empty() || reintroduceCount == 0) return;

        // Ensure we do not reintroduce more than available in the archive
        size_t count = std::min(reintroduceCount, archive.size());

        // Add the top X solutions from the archive to the new population
        newPopulation.insert(newPopulation.end(), archive.begin(), archive.begin() + count);

        // Optionally, remove reintroduced solutions from the archive
        // archive.erase(archive.begin(), archive.begin() + count);
    }

void selection() {
    // Sort the population by fitness in descending order to identify the elites
    std::sort(population.begin(), population.end(), [](const chromosome& a, const chromosome& b) {
        return a.fitness > b.fitness;
    });

    // Directly copy the eliteCount individuals to the new population
    std::vector<chromosome> newPopulation(population.begin(), population.begin() + eliteCount);

    // Reintroduce solutions from the archive to the new population
    reintroduceFromArchive(newPopulation);

    // Find the minimum fitness value among non-elites
    double minFitness = std::numeric_limits<double>::max();
    for (size_t i = eliteCount; i < population.size(); ++i) {
        if (population[i].fitness < minFitness) {
            minFitness = population[i].fitness;
        }
    }

    // Adjust fitness to ensure all values are non-negative
    double fitnessOffset = 0;
    if (minFitness < 0) {
        fitnessOffset = -minFitness;
    }

    // Calculate the total adjusted fitness of the non-elite population
    double totalAdjustedFitness = 0.0;
    for (size_t i = eliteCount; i < population.size(); ++i) {
        totalAdjustedFitness += population[i].fitness + fitnessOffset;
    }

    // Calculate the selection probability for each non-elite individual based on adjusted fitness
    std::vector<double> selectionProbabilities;
    for (size_t i = eliteCount; i < population.size(); ++i) {
        double selectionProbability = (population[i].fitness + fitnessOffset) / totalAdjustedFitness;
        selectionProbabilities.push_back(selectionProbability);
    }

    // Use std::discrete_distribution to select indices based on adjusted selection probabilities
    std::discrete_distribution<size_t> distribution(selectionProbabilities.begin(), selectionProbabilities.end());

    // Fill the remaining slots in the new population based on selection probabilities
    while (newPopulation.size() < populationSize) {
        size_t selectedIndex = distribution(gen);
        newPopulation.push_back(population[selectedIndex + eliteCount]);
    }

    // Update the population with the new generation, including elites
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
    }

    };