#pragma once
#include <string>
#include <bitset>
#include <random>
#include <iostream>
#include "chess.hpp"
#include "baselines.hpp"
#include "logger.hpp"



std::random_device rd;  // Obtain a random number from hardware
std::mt19937 gen(rd()); // Seed the generator


// the way I see it, there are three possible encoding schemas:
// 1. binary encoding -- a classic and universal GA encoding schema, but has the downside of 
// mutation causing too much change in the chromosome for a single bit
// (imagine a queen being 900, the msb is 512, and the next bit is 256, if the msb is flipped, the value changes by 512)
// 2. gray encoding -- a binary encoding where the bits are flipped in a way that the change in value is minimized, although this is more complicated and unfamiliar to implement. 
// luckily binary to gray and back is easy to implement, selection and mutation not as much
// 3. integer encoding -- the chromosome is a string of integers, and the mutation is done by adding or subtracting a random number from the integer. This also makes sense, but the mutation isn't as "genetic" as the other two methods, and this is not a popular option...
// https://www.cs.cmu.edu/Groups/AI/html/faqs/ai/genetic/part6/faq-doc-1.html#:~:text=A%20Gray%20code%20represents%20each,one%20bit%20at%20a%20time.

// because I don't like the idea of integer encoding, I will use binary encoding for the GA, and I will use gray encoding only if I think I have time and energy to implement it (or if I feel that binary encoding is not working well)

// there is also the issue of negative numbers. The only place where this is an issue is the piece square table values, and I'm tempted to omit them from the GA for now, and only tune the other values.
// upon further consideration, I am going to leave the piece square tables for now, so I can keep the ga unsigned. Consider 8 pieces, a middlegame and an endgame value, values between -256, 256, and 64 sqaures = 64 * 2 * 8 * 10 = 10,240 bits. consider conversely only 20 features, each feature a range of either 0-1028 (for piece values) or 0-256, which means 20 * either 10 or 8 = 160-200 bit length, a much better value. Each chromosone is now only 160-200 bits long, which is much more manageable. Append to that the piece values for each stage, you add another 10 * 8 * 2 = 160 bits, which is still manageable.

// takes in a binary string and returns the integer it represents (not Gray encoding)
// this is a helper function for the GA, that has a max of 10 bits
// int bitsToInt(const std::string& bits) {
//     // std::bitset allows for converting a binary string to an unsigned long
//     std::bitset<16> bitset(bits);
//     // return the integer value
//     return static_cast<int>(bitset.to_ulong());
// }


// Gray encoding stuff
// Function to convert binary to Gray code
unsigned int binaryToGray(unsigned int num) {
    return num ^ (num >> 1);
}

// Function to convert Gray code back to binary
unsigned int grayToBinary(unsigned int gray) {
    unsigned int binary = 0;
    for (; gray; gray = gray >> 1) {
        binary ^= gray;
    }
    return binary;
}

// takes in a Gray encoded binary string and returns the integer it represents
int bitsToInt(const std::string& bits) {
    std::bitset<16> gray(bits);
    unsigned int grayNum = static_cast<unsigned int>(gray.to_ulong());
    unsigned int binaryNum = grayToBinary(grayNum);
    return static_cast<int>(binaryNum);
}

// Function to generate a random integer within a given range
// takes in the num of bits, and whether the number is signed or not    
int randomInt(int bits) {
    std::uniform_int_distribution<int> distrib(0, (1 << bits) - 1);
    int randInt = distrib(gen);
    return randInt;
}

// a bit clunky, but it works...
// multiply piece values by ten so the are scaled correctly
TunableEval convertChromosoneToEval(const std::string& bitString){
    TunableEval tEval = baseEval; // defaultValues
    assert(baseEval.pawn.middleGame = 100); // check that the base eval values are correct (this is a sanity check to make sure the base eval values are correct, and that the random values are being added to the base eval values correctly)
    size_t pos = 0;

    // Parsing GamePhaseValue pawn (middleGame and endGame)
    tEval.pawn.middleGame = 100; // set to 100 always
    pos += 7;
    tEval.pawn.endGame = bitsToInt(bitString.substr(pos, 8));
    pos += 8;
    assert(tEval.pawn.middleGame != 0);
    tEval.knight.middleGame = bitsToInt(bitString.substr(pos, 9));
    pos += 9;
    tEval.knight.endGame = bitsToInt(bitString.substr(pos, 9));
    pos += 9;
    tEval.bishop.middleGame = bitsToInt(bitString.substr(pos, 9));
    pos += 9;
    tEval.bishop.endGame = bitsToInt(bitString.substr(pos, 9));
    pos += 9;
    tEval.rook.middleGame = bitsToInt(bitString.substr(pos, 10));
    pos += 10;
    tEval.rook.endGame = bitsToInt(bitString.substr(pos, 10));
    pos += 10;
    tEval.queen.middleGame = bitsToInt(bitString.substr(pos, 10));
    pos += 10;
    tEval.queen.endGame = bitsToInt(bitString.substr(pos, 10)); // these above values are multiplied by 10 to scale them correctly with the eval function and piece tables values (they will later be divided by 10 along w the piece values to get the correct value)
    pos += 10;
    tEval.passedPawn.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.passedPawn.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.doubledPawn.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.doubledPawn.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.isolatedPawn.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.isolatedPawn.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.weakPawn.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.weakPawn.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.centralPawn.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.centralPawn.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.weakSquare.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.weakSquare.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.passedPawnEnemyKingSquare.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.passedPawnEnemyKingSquare.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.knightOutposts.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.knightOutposts.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.knightMobility.middleGame = bitsToInt(bitString.substr(pos, 5));
    pos += 5;
    tEval.knightMobility.endGame = bitsToInt(bitString.substr(pos, 5));
    pos += 5;
    tEval.bishopMobility.middleGame = bitsToInt(bitString.substr(pos, 5));
    pos += 5;
    tEval.bishopMobility.endGame = bitsToInt(bitString.substr(pos, 5));
    pos += 5;
    tEval.bishopPair.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.bishopPair.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rookAttackKingFile.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rookAttackKingFile.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rookAttackKingAdjFile.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rookAttackKingAdjFile.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rook7thRank.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rook7thRank.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rookConnected.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rookConnected.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rookMobility.middleGame = bitsToInt(bitString.substr(pos, 5));
    pos += 5;
    tEval.rookMobility.endGame = bitsToInt(bitString.substr(pos, 5));
    pos += 5;
    tEval.rookBehindPassedPawn.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rookBehindPassedPawn.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rookOpenFile.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rookOpenFile.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rookSemiOpenFile.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rookSemiOpenFile.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rookAtckWeakPawnOpenColumn.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.rookAtckWeakPawnOpenColumn.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.queenMobility.middleGame = bitsToInt(bitString.substr(pos, 3));
    pos += 3;
    tEval.queenMobility.endGame = bitsToInt(bitString.substr(pos, 3));
    pos += 3;
    tEval.kingFriendlyPawn.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.kingFriendlyPawn.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.kingNoEnemyPawnNear.middleGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.kingNoEnemyPawnNear.endGame = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tEval.kingPressureScore.middleGame = bitsToInt(bitString.substr(pos, 4));
    pos += 4;
    tEval.kingPressureScore.endGame = bitsToInt(bitString.substr(pos, 4));
    pos += 4;

    // Return the populated struct
    return tEval;
}

TunableSearch convertChromosomeToSearch(const std::string& bitString){
    TunableSearch tSearch;
    int pos = 0;
    tSearch.aspirationWindowInitialDelta = bitsToInt(bitString.substr(pos, 6));
    pos += 6;
    tSearch.useAspirationWindowDepth = bitsToInt(bitString.substr(pos, 3));
    pos += 3;
    tSearch.useLazyEvalStatic = bitsToInt(bitString.substr(pos, 1));
    pos += 1;
    tSearch.futilityMargin = bitsToInt(bitString.substr(pos, 8));
    pos += 8;
    tSearch.razoringMargin = bitsToInt(bitString.substr(pos, 8));
    pos += 8;
    tSearch.deltaMargin = bitsToInt(bitString.substr(pos, 10));
    pos += 10;
    tSearch.killerMoveScore = bitsToInt(bitString.substr(pos, 10));
    pos += 10;
    tSearch.initalDepthLMR = bitsToInt(bitString.substr(pos, 3));
    pos += 3;
    tSearch.initialMoveCountLMR = bitsToInt(bitString.substr(pos, 3));
    pos += 3;
    tSearch.lmpMoveCount = bitsToInt(bitString.substr(pos, 4));
    pos += 4;
    tSearch.nullMovePruningInitialReduction = bitsToInt(bitString.substr(pos, 3));
    pos += 3;
    tSearch.nullMovePruningDepthFactor = bitsToInt(bitString.substr(pos, 5));
    pos += 5;

    return tSearch;
}

std::string convertSearchToChromosome(const TunableSearch& tSearch){
    std::string bitString = "";
    // Utility lambda to convert integer to Gray encoded binary string
    auto intToGrayString = [](int value, int bits) -> std::string {
        unsigned int grayValue = binaryToGray(static_cast<unsigned int>(value));
        return std::bitset<16>(grayValue).to_string().substr(16-bits, bits);
    };
    bitString += intToGrayString(tSearch.aspirationWindowInitialDelta, 6);
    bitString += intToGrayString(tSearch.useAspirationWindowDepth, 3);
    bitString += intToGrayString(tSearch.useLazyEvalStatic, 1);
    bitString += intToGrayString(tSearch.futilityMargin, 8);
    bitString += intToGrayString(tSearch.razoringMargin, 8);
    bitString += intToGrayString(tSearch.deltaMargin, 10);
    bitString += intToGrayString(tSearch.killerMoveScore, 10);
    bitString += intToGrayString(tSearch.initalDepthLMR, 3);
    bitString += intToGrayString(tSearch.initialMoveCountLMR, 3);
    bitString += intToGrayString(tSearch.lmpMoveCount, 4);
    bitString += intToGrayString(tSearch.nullMovePruningInitialReduction, 3);
    bitString += intToGrayString(tSearch.nullMovePruningDepthFactor, 5);
    
    return bitString;

}

TunableSearch initializeRandomTunableSearch() {
    TunableSearch rSearch = baseSearch;
    rSearch.aspirationWindowInitialDelta = randomInt(6);
    rSearch.useAspirationWindowDepth = randomInt(3);
    rSearch.useLazyEvalStatic = randomInt(1); // techncally this is a boolean
    rSearch.futilityMargin = randomInt(8);
    rSearch.razoringMargin = randomInt(8);
    rSearch.deltaMargin = randomInt(10);
    rSearch.killerMoveScore = randomInt(10);
    rSearch.initalDepthLMR = randomInt(3); 
    rSearch.initialMoveCountLMR = randomInt(3);
    rSearch.lmpMoveCount = randomInt(4);
    rSearch.nullMovePruningInitialReduction = randomInt(3);
    rSearch.nullMovePruningDepthFactor = randomInt(5);
    return rSearch;
}


// convert eval to chromosone for GA
// divide piece values by ten
std::string convertEvalToChromosone(const TunableEval& tEval){
    std::string bitString  = "";

    // Utility lambda to convert integer to Gray encoded binary string
    auto intToGrayString = [](int value, int bits) -> std::string {
        unsigned int grayValue = binaryToGray(static_cast<unsigned int>(value));
        return std::bitset<16>(grayValue).to_string().substr(16-bits, bits);
    };

    // Adding GamePhaseValue pawn (middleGame and endGame)
    bitString += intToGrayString(tEval.pawn.middleGame, 7); 
    bitString += intToGrayString(tEval.pawn.endGame, 8);

    assert(bitString.length() == 15); // check that the bitstring is the correct length (this is a sanity check to make sure the bitstring is the correct length, and that the random values are being added to the base eval values correctly

    bitString += intToGrayString(tEval.knight.middleGame, 9);
    bitString += intToGrayString(tEval.knight.endGame, 9);

    bitString += intToGrayString(tEval.bishop.middleGame, 9);
    bitString += intToGrayString(tEval.bishop.endGame, 9);

    bitString += intToGrayString(tEval.rook.middleGame, 10);
    bitString += intToGrayString(tEval.rook.endGame, 10);

    bitString += intToGrayString(tEval.queen.middleGame, 10);
    bitString += intToGrayString(tEval.queen.endGame, 10);

    // Repeat for all other values, for example:
    bitString += intToGrayString(tEval.passedPawn.middleGame, 6);
    bitString += intToGrayString(tEval.passedPawn.endGame, 6);

    bitString += intToGrayString(tEval.doubledPawn.middleGame, 6);
    bitString += intToGrayString(tEval.doubledPawn.endGame, 6);

    bitString += intToGrayString(tEval.isolatedPawn.middleGame, 6);
    bitString += intToGrayString(tEval.isolatedPawn.endGame, 6);

    bitString += intToGrayString(tEval.weakPawn.middleGame, 6);
    bitString += intToGrayString(tEval.weakPawn.endGame, 6);

    bitString += intToGrayString(tEval.centralPawn.middleGame, 6);
    bitString += intToGrayString(tEval.centralPawn.endGame, 6);

    bitString += intToGrayString(tEval.weakSquare.middleGame, 6);
    bitString += intToGrayString(tEval.weakSquare.endGame, 6);

    bitString += intToGrayString(tEval.passedPawnEnemyKingSquare.middleGame, 6);
    bitString += intToGrayString(tEval.passedPawnEnemyKingSquare.endGame, 6);

    bitString += intToGrayString(tEval.knightOutposts.middleGame, 6);
    bitString += intToGrayString(tEval.knightOutposts.endGame, 6);

    bitString += intToGrayString(tEval.knightMobility.middleGame, 5);
    bitString += intToGrayString(tEval.knightMobility.endGame, 5);

    bitString += intToGrayString(tEval.bishopMobility.middleGame, 5);
    bitString += intToGrayString(tEval.bishopMobility.endGame, 5);

    bitString += intToGrayString(tEval.bishopPair.middleGame, 6);
    bitString += intToGrayString(tEval.bishopPair.endGame, 6);

    bitString += intToGrayString(tEval.rookAttackKingFile.middleGame, 6);
    bitString += intToGrayString(tEval.rookAttackKingFile.endGame, 6);

    bitString += intToGrayString(tEval.rookAttackKingAdjFile.middleGame, 6);
    bitString += intToGrayString(tEval.rookAttackKingAdjFile.endGame, 6);

    bitString += intToGrayString(tEval.rook7thRank.middleGame, 6);
    bitString += intToGrayString(tEval.rook7thRank.endGame, 6);

    bitString += intToGrayString(tEval.rookConnected.middleGame, 6);
    bitString += intToGrayString(tEval.rookConnected.endGame, 6);

    bitString += intToGrayString(tEval.rookMobility.middleGame, 5);
    bitString += intToGrayString(tEval.rookMobility.endGame, 5);

    bitString += intToGrayString(tEval.rookBehindPassedPawn.middleGame, 6);
    bitString += intToGrayString(tEval.rookBehindPassedPawn.endGame, 6);

    bitString += intToGrayString(tEval.rookOpenFile.middleGame, 6);
    bitString += intToGrayString(tEval.rookOpenFile.endGame, 6);

    bitString += intToGrayString(tEval.rookSemiOpenFile.middleGame, 6);
    bitString += intToGrayString(tEval.rookSemiOpenFile.endGame, 6);

    bitString += intToGrayString(tEval.rookAtckWeakPawnOpenColumn.middleGame, 6);
    bitString += intToGrayString(tEval.rookAtckWeakPawnOpenColumn.endGame, 6);

    bitString += intToGrayString(tEval.kingFriendlyPawn.middleGame, 6);
    bitString += intToGrayString(tEval.kingFriendlyPawn.endGame, 6);

    bitString += intToGrayString(tEval.queenMobility.middleGame, 3);
    bitString += intToGrayString(tEval.queenMobility.endGame, 3);

    bitString += intToGrayString(tEval.kingNoEnemyPawnNear.middleGame, 6);
    bitString += intToGrayString(tEval.kingNoEnemyPawnNear.endGame, 6);

    bitString += intToGrayString(tEval.kingPressureScore.middleGame, 4);
    bitString += intToGrayString(tEval.kingPressureScore.endGame, 4);


    return bitString;
}





// Function to initialize a TunableEval struct with random values
// could potentially speed this up by not creating a struct but just making a random string with the 
// first seven bits 0110010 (100 in binary) and the rest random
TunableEval initializeRandomTunableEval() {
    TunableEval tEval = baseEval; // get global base eval values

    // Initialize GamePhaseValues with random values, assuming a range
    tEval.pawn = GamePhaseValue(100, randomInt(8)); // 0 - 256 range, hard coded to 100 to allow other values to be interepretable to the base pawn value
    tEval.knight = GamePhaseValue(randomInt(9), randomInt(9)); // 0 - 512 range
    tEval.bishop = GamePhaseValue(randomInt(9), randomInt(9)); // 0 - 512 range
    tEval.rook = GamePhaseValue(randomInt(10), randomInt(10)); // 0 - 1024 range
    tEval.queen = GamePhaseValue(randomInt(10), randomInt(10)); // 0 - 1024 range
    tEval.passedPawn = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.doubledPawn = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.isolatedPawn = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.weakPawn = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.weakSquare = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.passedPawnEnemyKingSquare = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.knightOutposts = GamePhaseValue(randomInt(5), randomInt(5)); // 0 - 128 range
    tEval.knightMobility = GamePhaseValue(randomInt(5), randomInt(5)); // 0 - 31 range
    tEval.bishopMobility = GamePhaseValue(randomInt(5), randomInt(5)); // 0 - 128 range
    tEval.bishopPair = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.rookAttackKingFile = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.rookAttackKingAdjFile = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.rook7thRank = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.rookConnected = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.rookMobility = GamePhaseValue(randomInt(5), randomInt(5)); // 0 - 128 range
    tEval.rookBehindPassedPawn = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.rookOpenFile = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.rookSemiOpenFile = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.rookAtckWeakPawnOpenColumn = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.queenMobility = GamePhaseValue(randomInt(3), randomInt(3)); // 0 - 7 range
    tEval.kingFriendlyPawn = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.kingNoEnemyPawnNear = GamePhaseValue(randomInt(6), randomInt(6)); // 0 - 128 range
    tEval.kingPressureScore = GamePhaseValue(randomInt(4), randomInt(4)); // 0 - 128 range


    return tEval;
}



// // so I can print out my king safety table
template <typename S>
std::ostream& operator<<(std::ostream& os,
                    const std::vector<S>& vector)
{
    // Printing all the elements
    // using <<
    for (auto element : vector) {
        os << element << " ";
    }
    return os;
}


void printTunableSearch(const TunableSearch& tSearch){
    std::ostringstream logMsg = std::ostringstream();
    logMsg << "TunableSearch resultX = {\n";
    logMsg << tSearch.aspirationWindowInitialDelta << ", // Aspiration Window initial delta\n";
    logMsg << tSearch.useAspirationWindowDepth << ", // Use Aspiration Window Depth\n";
    logMsg << tSearch.useLazyEvalStatic << ", // Use Lazy Eval Static\n";
    logMsg << tSearch.futilityMargin << ", // Futility Margin \n";
    logMsg << tSearch.razoringMargin << ", // Razoring Margin \n";
    logMsg << tSearch.deltaMargin << ", // Delta Margin\n";
    logMsg << tSearch.killerMoveScore << ", // Killer Move Score\n";
    logMsg << tSearch.initalDepthLMR << ", // Initial LMR Depth\n"; // Remember, there's a typo here in the original struct definition: "inital" should be "initial"
    logMsg << tSearch.initialMoveCountLMR << ", // Initial Move Counter\n";
    logMsg << tSearch.lmpMoveCount << ", // LMP Move Count\n";
    logMsg << tSearch.nullMovePruningInitialReduction << ", // Null Move Pruning Initial Reduction\n";
    logMsg << tSearch.nullMovePruningDepthFactor << ", // Null Move Pruning Depth Factor\n";
    logMsg << "};\n";
    Logger::getInstance().log(logMsg.str());
    std::cout << logMsg.str();
}


void printTunableEval(const TunableEval& tEval) {
    std::ostringstream logMsg = std::ostringstream();

    logMsg << "TunableEval baseEval = {\n";
    logMsg << "    chess::constants::STARTPOS,\n";
    // Piece values
    logMsg << "    GamePhaseValue(" << tEval.pawn.middleGame << ", " << tEval.pawn.endGame << "), // Pawn\n";
    logMsg << "    GamePhaseValue(" << tEval.knight.middleGame << ", " << tEval.knight.endGame << "), // Knight\n";
    logMsg << "    GamePhaseValue(" << tEval.bishop.middleGame << ", " << tEval.bishop.endGame << "), // Bishop\n";
    logMsg << "    GamePhaseValue(" << tEval.rook.middleGame << ", " << tEval.rook.endGame << "), // Rook\n";
    logMsg << "    GamePhaseValue(" << tEval.queen.middleGame << ", " << tEval.queen.endGame << "), // Queen\n";
    // Other values based on whims
    logMsg << "    GamePhaseValue(" << tEval.passedPawn.middleGame << ", " << tEval.passedPawn.endGame << "), // Passed Pawn\n";
    logMsg << "    GamePhaseValue(" << tEval.doubledPawn.middleGame << ", " << tEval.doubledPawn.endGame << "), // Doubled Pawn\n";
    logMsg << "    GamePhaseValue(" << tEval.isolatedPawn.middleGame << ", " << tEval.isolatedPawn.endGame << "), // Isolated Pawn\n";
    logMsg << "    GamePhaseValue(" << tEval.weakPawn.middleGame << ", " << tEval.weakPawn.endGame << "), // Weak Pawn\n";
    logMsg << "    GamePhaseValue(" << tEval.centralPawn.middleGame << ", " << tEval.centralPawn.endGame << "), // Central Pawn\n";
    logMsg << "    GamePhaseValue(" << tEval.weakSquare.middleGame << ", " << tEval.weakSquare.endGame << "), // Weak Square\n";
    logMsg << "    GamePhaseValue(" << tEval.passedPawnEnemyKingSquare.middleGame << ", " << tEval.passedPawnEnemyKingSquare.endGame << "), // Passed Pawn Enemy King Square\n";
    logMsg << "    GamePhaseValue(" << tEval.knightOutposts.middleGame << ", " << tEval.knightOutposts.endGame << "), // Knight Outposts\n";
    logMsg << "    GamePhaseValue(" << tEval.knightMobility.middleGame << ", " << tEval.knightMobility.endGame << "), // Knight Mobility\n";
    logMsg << "    GamePhaseValue(" << tEval.bishopMobility.middleGame << ", " << tEval.bishopMobility.endGame << "), // Bishop Mobility\n";
    logMsg << "    GamePhaseValue(" << tEval.bishopPair.middleGame << ", " << tEval.bishopPair.endGame << "), // Bishop Pair\n";
    logMsg << "    GamePhaseValue(" << tEval.rookAttackKingFile.middleGame << ", " << tEval.rookAttackKingFile.endGame << "), // Rook Attack King File\n";
    logMsg << "    GamePhaseValue(" << tEval.rookAttackKingAdjFile.middleGame << ", " << tEval.rookAttackKingAdjFile.endGame << "), // Rook Attack King Adjacent File\n";
    logMsg << "    GamePhaseValue(" << tEval.rook7thRank.middleGame << ", " << tEval.rook7thRank.endGame << "), // Rook 7th Rank\n";
    logMsg << "    GamePhaseValue(" << tEval.rookConnected.middleGame << ", " << tEval.rookConnected.endGame << "), // Rook Connected\n";
    logMsg << "    GamePhaseValue(" << tEval.rookMobility.middleGame << ", " << tEval.rookMobility.endGame << "), // Rook Mobility\n";
    logMsg << "    GamePhaseValue(" << tEval.rookBehindPassedPawn.middleGame << ", " << tEval.rookBehindPassedPawn.endGame << "), // Rook Behind Passed Pawn\n";
    logMsg << "    GamePhaseValue(" << tEval.rookOpenFile.middleGame << ", " << tEval.rookOpenFile.endGame << "), // Rook Open File\n";
    logMsg << "    GamePhaseValue(" << tEval.rookSemiOpenFile.middleGame << ", " << tEval.rookSemiOpenFile.endGame << "), // Rook Semi Open File\n";
    logMsg << "    GamePhaseValue(" << tEval.rookAtckWeakPawnOpenColumn.middleGame << ", " << tEval.rookAtckWeakPawnOpenColumn.endGame << "), // Rook Attack Weak Pawn Open Column\n";
    logMsg << "    GamePhaseValue(" << tEval.kingFriendlyPawn.middleGame << ", " << tEval.kingFriendlyPawn.endGame << "), // King Friendly Pawn\n";
    logMsg << "    GamePhaseValue(" << tEval.kingNoEnemyPawnNear.middleGame << ", " << tEval.kingNoEnemyPawnNear.endGame << "), // King No Enemy Pawn Near\n";
    logMsg << "    GamePhaseValue(" << tEval.queenMobility.middleGame << ", " << tEval.queenMobility.endGame << "), // Queen Mobility\n";
    // King safety
    logMsg << "    GamePhaseValue(" << tEval.kingPressureScore.middleGame << ", " << tEval.kingPressureScore.endGame << "), // King Pressure Score\n";
    logMsg << "};\n";
    Logger::getInstance().log(logMsg.str());
    std::cout << logMsg.str();
}










