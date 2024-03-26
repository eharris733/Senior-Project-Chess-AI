#pragma once
#include "baselines.hpp"
#include "chess.hpp"

/*
Population Size: 1000
Initial Mutation Rate: 0.002
Mutation Decay Rate: 0
Crossover Rate: 0.75
Total Generations: 300
Training Size: 1000
Elite Size: 0
Archive Size: 0
Replacement Count: 0
Training Data Pathdbs/stockfish11.csv*/
TunableEval ga1result1 = {
    chess::constants::STARTPOS,
    GamePhaseValue(100, 137), // Pawn
    GamePhaseValue(367, 301), // Knight
    GamePhaseValue(394, 327), // Bishop
    GamePhaseValue(549, 510), // Rook
    GamePhaseValue(1018, 994), // Queen
    GamePhaseValue(15, 27), // Passed Pawn
    GamePhaseValue(8, 61), // Doubled Pawn
    GamePhaseValue(9, 11), // Isolated Pawn
    GamePhaseValue(6, 10), // Weak Pawn
    GamePhaseValue(9, 2), // Central Pawn
    GamePhaseValue(1, 7), // Weak Square
    GamePhaseValue(0, 26), // Passed Pawn Enemy King Square
    GamePhaseValue(1, 16), // Knight Outposts
    GamePhaseValue(12, 4), // Knight Mobility
    GamePhaseValue(10, 3), // Bishop Mobility
    GamePhaseValue(27, 25), // Bishop Pair
    GamePhaseValue(11, 25), // Rook Attack King File
    GamePhaseValue(18, 42), // Rook Attack King Adjacent File
    GamePhaseValue(26, 20), // Rook 7th Rank
    GamePhaseValue(19, 28), // Rook Connected
    GamePhaseValue(6, 0), // Rook Mobility
    GamePhaseValue(15, 42), // Rook Behind Passed Pawn
    GamePhaseValue(32, 21), // Rook Open File
    GamePhaseValue(11, 22), // Rook Semi Open File
    GamePhaseValue(8, 60), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(13, 6), // King Friendly Pawn
    GamePhaseValue(12, 0), // King No Enemy Pawn Near
    GamePhaseValue(6, 7), // Queen Mobility
    GamePhaseValue(1, 0), // King Pressure Score
};

/*
Population Size: 1000
Initial Mutation Rate: 0.02
Mutation Decay Rate: 0.01
Crossover Rate: 0.75
Total Generations: 300
Training Size: 1000
Elite Size: 10
Archive Size: 1000
Replacement Count: 10
Training Data Pathdbs/stockfish11.csv
*/
TunableEval ga1result2 = {
 chess::constants::STARTPOS,
    GamePhaseValue(100, 104), // Pawn
    GamePhaseValue(368, 276), // Knight
    GamePhaseValue(393, 348), // Bishop
    GamePhaseValue(571, 486), // Rook
    GamePhaseValue(1018, 1020), // Queen
    GamePhaseValue(11, 32), // Passed Pawn
    GamePhaseValue(46, 10), // Doubled Pawn
    GamePhaseValue(16, 3), // Isolated Pawn
    GamePhaseValue(5, 27), // Weak Pawn
    GamePhaseValue(36, 13), // Central Pawn
    GamePhaseValue(2, 5), // Weak Square
    GamePhaseValue(2, 3), // Passed Pawn Enemy King Square
    GamePhaseValue(50, 6), // Knight Outposts
    GamePhaseValue(15, 11), // Knight Mobility
    GamePhaseValue(6, 8), // Bishop Mobility
    GamePhaseValue(62, 62), // Bishop Pair
    GamePhaseValue(55, 10), // Rook Attack King File
    GamePhaseValue(3, 55), // Rook Attack King Adjacent File
    GamePhaseValue(40, 48), // Rook 7th Rank
    GamePhaseValue(0, 42), // Rook Connected
    GamePhaseValue(2, 5), // Rook Mobility
    GamePhaseValue(15, 14), // Rook Behind Passed Pawn
    GamePhaseValue(33, 4), // Rook Open File
    GamePhaseValue(8, 1), // Rook Semi Open File
    GamePhaseValue(49, 11), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(13, 3), // King Friendly Pawn
    GamePhaseValue(11, 4), // King No Enemy Pawn Near
    GamePhaseValue(5, 5), // Queen Mobility
    GamePhaseValue(0, 2), // King Pressure Score
};
/*
Population Size: 1000
Initial Mutation Rate: 0.02
Mutation Decay Rate: 0.05
Crossover Rate: 0.75
Total Generations: 300
Training Size: 1000
Elite Size: 0
Archive Size: 2000
Replacement Count: 100
Training Data Pathdbs/stockfish11.csv*/
TunableEval ga1result3 = {
    chess::constants::STARTPOS,
    GamePhaseValue(100, 99), // Pawn
    GamePhaseValue(335, 251), // Knight
    GamePhaseValue(398, 276), // Bishop
    GamePhaseValue(572, 520), // Rook
    GamePhaseValue(1018, 1020), // Queen
    GamePhaseValue(12, 49), // Passed Pawn
    GamePhaseValue(63, 59), // Doubled Pawn
    GamePhaseValue(21, 15), // Isolated Pawn
    GamePhaseValue(2, 3), // Weak Pawn
    GamePhaseValue(11, 0), // Central Pawn
    GamePhaseValue(2, 6), // Weak Square
    GamePhaseValue(2, 0), // Passed Pawn Enemy King Square
    GamePhaseValue(3, 25), // Knight Outposts
    GamePhaseValue(14, 20), // Knight Mobility
    GamePhaseValue(5, 13), // Bishop Mobility
    GamePhaseValue(26, 62), // Bishop Pair
    GamePhaseValue(19, 26), // Rook Attack King File
    GamePhaseValue(7, 2), // Rook Attack King Adjacent File
    GamePhaseValue(50, 35), // Rook 7th Rank
    GamePhaseValue(15, 31), // Rook Connected
    GamePhaseValue(1, 5), // Rook Mobility
    GamePhaseValue(9, 29), // Rook Behind Passed Pawn
    GamePhaseValue(48, 9), // Rook Open File
    GamePhaseValue(35, 20), // Rook Semi Open File
    GamePhaseValue(51, 17), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(14, 2), // King Friendly Pawn
    GamePhaseValue(2, 0), // King No Enemy Pawn Near
    GamePhaseValue(7, 7), // Queen Mobility
    GamePhaseValue(1, 1), // King Pressure Score
};

/*

Population Size: 1000
Initial Mutation Rate: 0.02
Mutation Decay Rate: 0.05
Crossover Rate: 0.75
Total Generations: 300
Training Size: 1000
Elite Size: 0
Archive Size: 2000
Replacement Count: 100
Training Data Pathdbs/stockfish11.csv
*** note that this dataset used a less stringent filter, doing less than 5 pieces and less than 8000 absolute eval
*/
TunableEval ga1result4 = {
    chess::constants::STARTPOS,
    GamePhaseValue(100, 124), // Pawn
    GamePhaseValue(356, 244), // Knight
    GamePhaseValue(359, 332), // Bishop
    GamePhaseValue(544, 563), // Rook
    GamePhaseValue(1021, 1018), // Queen
    GamePhaseValue(9, 30), // Passed Pawn
    GamePhaseValue(26, 55), // Doubled Pawn
    GamePhaseValue(21, 15), // Isolated Pawn
    GamePhaseValue(1, 5), // Weak Pawn
    GamePhaseValue(11, 1), // Central Pawn
    GamePhaseValue(1, 9), // Weak Square
    GamePhaseValue(3, 11), // Passed Pawn Enemy King Square
    GamePhaseValue(12, 28), // Knight Outposts
    GamePhaseValue(10, 17), // Knight Mobility
    GamePhaseValue(8, 7), // Bishop Mobility
    GamePhaseValue(59, 59), // Bishop Pair
    GamePhaseValue(21, 8), // Rook Attack King File
    GamePhaseValue(12, 0), // Rook Attack King Adjacent File
    GamePhaseValue(24, 6), // Rook 7th Rank
    GamePhaseValue(11, 40), // Rook Connected
    GamePhaseValue(1, 2), // Rook Mobility
    GamePhaseValue(1, 42), // Rook Behind Passed Pawn
    GamePhaseValue(58, 28), // Rook Open File
    GamePhaseValue(33, 27), // Rook Semi Open File
    GamePhaseValue(38, 54), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(11, 2), // King Friendly Pawn
    GamePhaseValue(4, 1), // King No Enemy Pawn Near
    GamePhaseValue(7, 7), // Queen Mobility
    GamePhaseValue(2, 1), // King Pressure Score
};

/*
Population Size: 1000
Initial Mutation Rate: 0.02
Mutation Decay Rate: 0.05
Crossover Rate: 0.75
Total Generations: 300
Training Size: 1000
Elite Size: 0
Archive Size: 2000
Replacement Count: 100
Training Data Pathdbs/stockfish11.csv
*** note that this was usign alls the data, taking from gen 180
*/
TunableEval ga1result5 = {
chess::constants::STARTPOS,
    GamePhaseValue(100, 116), // Pawn
    GamePhaseValue(372, 270), // Knight
    GamePhaseValue(427, 271), // Bishop
    GamePhaseValue(533, 498), // Rook
    GamePhaseValue(1008, 991), // Queen
    GamePhaseValue(6, 23), // Passed Pawn
    GamePhaseValue(62, 56), // Doubled Pawn
    GamePhaseValue(20, 11), // Isolated Pawn
    GamePhaseValue(3, 18), // Weak Pawn
    GamePhaseValue(13, 1), // Central Pawn
    GamePhaseValue(1, 6), // Weak Square
    GamePhaseValue(2, 12), // Passed Pawn Enemy King Square
    GamePhaseValue(5, 20), // Knight Outposts
    GamePhaseValue(8, 11), // Knight Mobility
    GamePhaseValue(4, 10), // Bishop Mobility
    GamePhaseValue(1, 58), // Bishop Pair
    GamePhaseValue(9, 30), // Rook Attack King File
    GamePhaseValue(1, 7), // Rook Attack King Adjacent File
    GamePhaseValue(30, 2), // Rook 7th Rank
    GamePhaseValue(13, 21), // Rook Connected
    GamePhaseValue(0, 2), // Rook Mobility
    GamePhaseValue(1, 26), // Rook Behind Passed Pawn
    GamePhaseValue(29, 19), // Rook Open File
    GamePhaseValue(15, 20), // Rook Semi Open File
    GamePhaseValue(32, 48), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(11, 1), // King Friendly Pawn
    GamePhaseValue(4, 0), // King No Enemy Pawn Near
    GamePhaseValue(0, 6), // Queen Mobility
    GamePhaseValue(4, 2), // King Pressure Score
};

//same run as above, final values

TunableEval ga1result6 ={
    chess::constants::STARTPOS,
    GamePhaseValue(100, 116), // Pawn
    GamePhaseValue(372, 267), // Knight
    GamePhaseValue(421, 271), // Bishop
    GamePhaseValue(541, 506), // Rook
    GamePhaseValue(1013, 944), // Queen
    GamePhaseValue(6, 22), // Passed Pawn
    GamePhaseValue(53, 56), // Doubled Pawn
    GamePhaseValue(20, 11), // Isolated Pawn
    GamePhaseValue(3, 2), // Weak Pawn
    GamePhaseValue(13, 1), // Central Pawn
    GamePhaseValue(1, 6), // Weak Square
    GamePhaseValue(1, 3), // Passed Pawn Enemy King Square
    GamePhaseValue(4, 21), // Knight Outposts
    GamePhaseValue(8, 11), // Knight Mobility
    GamePhaseValue(4, 10), // Bishop Mobility
    GamePhaseValue(2, 58), // Bishop Pair
    GamePhaseValue(9, 30), // Rook Attack King File
    GamePhaseValue(1, 7), // Rook Attack King Adjacent File
    GamePhaseValue(28, 5), // Rook 7th Rank
    GamePhaseValue(22, 10), // Rook Connected
    GamePhaseValue(0, 3), // Rook Mobility
    GamePhaseValue(0, 59), // Rook Behind Passed Pawn
    GamePhaseValue(29, 19), // Rook Open File
    GamePhaseValue(15, 20), // Rook Semi Open File
    GamePhaseValue(32, 48), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(11, 1), // King Friendly Pawn
    GamePhaseValue(3, 0), // King No Enemy Pawn Near
    GamePhaseValue(0, 6), // Queen Mobility
    GamePhaseValue(2, 0), // King Pressure Score
};

// double initialMutationRate = 0.03;
    // double decayRate = 0.05;
    // double crossoverRate = 0.75;
    // int totalGenerations = 300;
    // size_t populationSize = 2000; 
    // int trainingSize = 1000;
    // int eliteSize = 0;
    // int archiveSize = 2000;
    // int replacementCount = 100;
    // std::string trainingDataPath = "dbs/stockfish11.csv";
TunableEval ga1result7 = {
    chess::constants::STARTPOS,
    GamePhaseValue(100, 99), // Pawn
    GamePhaseValue(351, 220), // Knight
    GamePhaseValue(359, 249), // Bishop
    GamePhaseValue(529, 489), // Rook
    GamePhaseValue(1009, 984), // Queen
    GamePhaseValue(5, 31), // Passed Pawn
    GamePhaseValue(55, 30), // Doubled Pawn
    GamePhaseValue(19, 11), // Isolated Pawn
    GamePhaseValue(8, 0), // Weak Pawn
    GamePhaseValue(9, 5), // Central Pawn
    GamePhaseValue(0, 4), // Weak Square
    GamePhaseValue(8, 1), // Passed Pawn Enemy King Square
    GamePhaseValue(3, 19), // Knight Outposts
    GamePhaseValue(10, 16), // Knight Mobility
    GamePhaseValue(6, 12), // Bishop Mobility
    GamePhaseValue(52, 50), // Bishop Pair
    GamePhaseValue(9, 9), // Rook Attack King File
    GamePhaseValue(2, 1), // Rook Attack King Adjacent File
    GamePhaseValue(21, 30), // Rook 7th Rank
    GamePhaseValue(26, 7), // Rook Connected
    GamePhaseValue(1, 4), // Rook Mobility
    GamePhaseValue(16, 24), // Rook Behind Passed Pawn
    GamePhaseValue(40, 15), // Rook Open File
    GamePhaseValue(18, 14), // Rook Semi Open File
    GamePhaseValue(58, 35), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(10, 2), // King Friendly Pawn
    GamePhaseValue(4, 0), // King No Enemy Pawn Near
    GamePhaseValue(1, 6), // Queen Mobility
    GamePhaseValue(2, 0), // King Pressure Score
};

// results from gen 200, larger training size
// double initialMutationRate = 0.03;
    // double decayRate = 0.05;
    // double crossoverRate = 0.75;
    // int totalGenerations = 300;
    // size_t populationSize = 1000; 
    // int trainingSize = 2000;
    // int eliteSize = 0;
    // int archiveSize = 2000;
    // int replacementCount = 100;
    // std::string trainingDataPath = "dbs/stockfish11.csv";
TunableEval ga1result8 = {
    chess::constants::STARTPOS,
    GamePhaseValue(100, 88), // Pawn
    GamePhaseValue(325, 221), // Knight
    GamePhaseValue(358, 307), // Bishop
    GamePhaseValue(503, 472), // Rook
    GamePhaseValue(1013, 929), // Queen
    GamePhaseValue(5, 42), // Passed Pawn
    GamePhaseValue(23, 50), // Doubled Pawn
    GamePhaseValue(21, 15), // Isolated Pawn
    GamePhaseValue(4, 22), // Weak Pawn
    GamePhaseValue(10, 11), // Central Pawn
    GamePhaseValue(1, 4), // Weak Square
    GamePhaseValue(0, 2), // Passed Pawn Enemy King Square
    GamePhaseValue(12, 28), // Knight Outposts
    GamePhaseValue(13, 15), // Knight Mobility
    GamePhaseValue(6, 5), // Bishop Mobility
    GamePhaseValue(51, 60), // Bishop Pair
    GamePhaseValue(21, 1), // Rook Attack King File
    GamePhaseValue(10, 16), // Rook Attack King Adjacent File
    GamePhaseValue(22, 25), // Rook 7th Rank
    GamePhaseValue(22, 41), // Rook Connected
    GamePhaseValue(1, 6), // Rook Mobility
    GamePhaseValue(14, 31), // Rook Behind Passed Pawn
    GamePhaseValue(45, 8), // Rook Open File
    GamePhaseValue(13, 22), // Rook Semi Open File
    GamePhaseValue(12, 61), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(10, 4), // King Friendly Pawn
    GamePhaseValue(4, 0), // King No Enemy Pawn Near
    GamePhaseValue(1, 4), // Queen Mobility
    GamePhaseValue(2, 0), // King Pressure Score
};

// results from full run
// double initialMutationRate = 0.03;
    // double decayRate = 0.05;
    // double crossoverRate = 0.75;
    // int totalGenerations = 300;
    // size_t populationSize = 1000; 
    // int trainingSize = 2000;
    // int eliteSize = 0;
    // int archiveSize = 2000;
    // int replacementCount = 100;
    // std::string trainingDataPath = "dbs/stockfish11.csv";

TunableEval ga1result10  {
    chess::constants::STARTPOS,
    GamePhaseValue(100, 89), // Pawn
    GamePhaseValue(330, 226), // Knight
    GamePhaseValue(359, 307), // Bishop
    GamePhaseValue(506, 479), // Rook
    GamePhaseValue(1021, 991), // Queen
    GamePhaseValue(4, 42), // Passed Pawn
    GamePhaseValue(20, 45), // Doubled Pawn
    GamePhaseValue(20, 15), // Isolated Pawn
    GamePhaseValue(3, 18), // Weak Pawn
    GamePhaseValue(14, 3), // Central Pawn
    GamePhaseValue(1, 3), // Weak Square
    GamePhaseValue(0, 2), // Passed Pawn Enemy King Square
    GamePhaseValue(3, 28), // Knight Outposts
    GamePhaseValue(12, 15), // Knight Mobility
    GamePhaseValue(6, 5), // Bishop Mobility
    GamePhaseValue(39, 60), // Bishop Pair
    GamePhaseValue(22, 1), // Rook Attack King File
    GamePhaseValue(10, 10), // Rook Attack King Adjacent File
    GamePhaseValue(2, 27), // Rook 7th Rank
    GamePhaseValue(22, 41), // Rook Connected
    GamePhaseValue(1, 6), // Rook Mobility
    GamePhaseValue(1, 14), // Rook Behind Passed Pawn
    GamePhaseValue(45, 7), // Rook Open File
    GamePhaseValue(26, 10), // Rook Semi Open File
    GamePhaseValue(45, 61), // Rook Attack Weak Pawn Open Column
    GamePhaseValue(11, 4), // King Friendly Pawn
    GamePhaseValue(4, 0), // King No Enemy Pawn Near
    GamePhaseValue(1, 4), // Queen Mobility
    GamePhaseValue(2, 0), // King Pressure Score
};