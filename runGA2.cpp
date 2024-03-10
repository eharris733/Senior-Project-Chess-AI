#include "ga_util.hpp"
#include "ga2.hpp"
#include "chess.hpp"
#include <string>
#include <cassert> // Add missing include for assert
#include <chrono>
#include <algorithm>


int main(){
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    double mutationThreshold = 0.2;
    double mutationRate = 0.02;
    int totalGenerations = 200;
    size_t populationSize = 20; 
    int trainingSize = 1000;
    std::string trainingDataPath = "dbs/mastergames/ga2_test.csv";

    std::cout << "Running Genetic Algorithm" << std::endl;
    std::cout << "Population Size: " << populationSize << std::endl;
    std::cout << "Mutation Threshold: " << mutationThreshold << std::endl;
    std::cout << "Mutation Rate: " << mutationRate << std::endl;
    std::cout << "Total Generations: " << totalGenerations << std::endl;
    std::cout << "Training Size: " << trainingSize << std::endl;
    std::cout << "Training Data Path: " << trainingDataPath << std::endl;


    
    GA2 GA = GA2(trainingDataPath, populationSize, mutationThreshold, mutationRate, totalGenerations, trainingSize);
    GA.run();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " seconds" << std::endl;
    return 0;

}