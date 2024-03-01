#pragma once
#include <string>
#include <bitset>

// this file is the start for the preparation of a GA
// ideally all values that should be tuned will be in this file
// more or less as a global variable

// eval function weights
// a struct to allow each feature to be weighted differently based on phase of the game
struct GamePhaseValue {
    int middleGame;
    int endGame;

    GamePhaseValue(int mg = 0, int eg = 0) : middleGame(mg), endGame(eg) {}
};

// define the struct here
struct TunableEval{
    std::string fen; // The FEN string
    int pawnsMG[64];
    int pawnsEG[64];
    int knightsMG[64];
    int knightsEG[64];
    int bishopsMG[64];
    int bishopsEG[64];
    int rooksMG[64];
    int rooksEG[64];
    int queensMG[64];
    int queensEG[64];
    int kingsMG[64];
    int kingsEG[64];
    GamePhaseValue pawn;
    GamePhaseValue knight;
    GamePhaseValue bishop;
    GamePhaseValue rook;
    GamePhaseValue queen;
    GamePhaseValue passedPawn;
    GamePhaseValue doubledPawn;
    GamePhaseValue isolatedPawn;
    GamePhaseValue weakPawn;
    GamePhaseValue weakSquare;
    GamePhaseValue passedPawnEnemyKingSquare;
    GamePhaseValue knightOutposts;
    GamePhaseValue bishopMobility;
    GamePhaseValue bishopPair;
    GamePhaseValue rookAttackKingFile;
    GamePhaseValue rookAttackKingAdjFile;
    GamePhaseValue rook7thRank;
    GamePhaseValue rookConnected;
    GamePhaseValue rookMobility;
    GamePhaseValue rookBehindPassedPawn;
    GamePhaseValue rookOpenFile;
    GamePhaseValue rookSemiOpenFile;
    GamePhaseValue rookAtckWeakPawnOpenColumn;
    GamePhaseValue kingFriendlyPawn;
    GamePhaseValue kingNoEnemyPawnNear;
    GamePhaseValue kingPressureScore;

  

};



struct TunableSearch{
    //tuneable search parameters

    // for aspiration window
    int aspirationWindowProgression[2];
    int aspirationWindowInitialDelta;
    int useAspirationWindowDepth;

    // for null move reductions
    bool useLazyEvalNMR;

    // for futility pruning
    int futilityMargin[3];
    bool useLazyEvalFutility;
    // for delta pruning in QS search
    int deltaMargin;

    // for move ordering
    int promotionMoveScore;
    int killerMoveScore;
    int baseScore;

    //for late move reductions
    int initalDepthLMR;
    int secondaryDepthLMR;
    int initialMoveCountLMR;
    int secondaryMoveCountLMR;
};


int bitsToInt(const std::string& bits) {
    // std::bitset allows for converting a binary string to an unsigned long
    std::bitset<32> bitset(bits);
    return static_cast<int>(bitset.to_ulong());
}

// multiply piece values by ten so the are scaled correctly
TunableEval convertChromosoneToEval(const std::string& bitString){
    TunableEval tEval;
    size_t pos = 0;

    // Example for pawnsMG[0]
    // Assuming each int is represented by 32 bits in the bitString
    tEval.pawnsMG[0] = bitsToInt(bitString.substr(pos, 32));
    pos += 32;

    // Parsing GamePhaseValue pawn (middleGame and endGame)
    tEval.pawn.middleGame = bitsToInt(bitString.substr(pos, 32));
    pos += 32;

    tEval.pawn.endGame = bitsToInt(bitString.substr(pos, 32));
    pos += 32;

    // Additional fields would be parsed in a similar manner...

    // Return the populated struct
    return tEval;
}


// convert eval to chromosone for GA
// divide piece values by ten
string convertEvalToChromosone(const TunableEval& tEval){
    std::string bitString;

    // Example for pawnsMG[0]
    // Assuming each int is represented by 32 bits in the bitString
    bitString += std::bitset<32>(tEval.pawnsMG[0]).to_string();

    // Adding GamePhaseValue pawn (middleGame and endGame)
    bitString += std::bitset<32>(tEval.pawn.middleGame).to_string();
    bitString += std::bitset<32>(tEval.pawn.endGame).to_string();

    // Additional fields would be added in a similar manner...

    return bitString;
}

std::random_device rd;  // Obtain a random number from hardware
std::mt19937 gen(rd()); // Seed the generator


// Function to generate a random integer within a given range
// takes in the num of bits, and whether the number is signed or not    
int randomInt(int bits, bool isSigned) {
    std::uniform_int_distribution<int> distrib(0, (1 << bits) - 1);
    int randInt = distrib(gen);
    if (isSigned) {
        // If the number is signed, we need to convert it to a negative number half the time
        if (randInt & (1 << (bits - 1))) {
            randInt -= (1 << bits);
        }
    }
    return randInt;
}

// Function to initialize a TunableEval struct with random values
TunableEval initializeRandomTunableEval() {
    TunableEval tEval;

    // For simplicity, let's say each square's value ranges from -100 to 100
    for (int i = 0; i < 64; ++i) {
        tEval.pawnsMG[i] = randomInt(10, true); // up to ten bits, and signed = -256 - 256 range
        tEval.pawnsEG[i] = randomInt(10, true);
        // Repeat for other pieces...
    }

    // Initialize GamePhaseValues with random values, assuming a range
    tEval.pawn = GamePhaseValue(100, randomInt(10, false)); // 0 - 1024 range, hard coded to 100 to allow other values to be tested
    // Repeat for other GamePhaseValues...

    // For the sake of example, other fields are initialized similarly
    // ...

    return tEval;
}






