#include "tunableValues.hpp"
#include "chess.hpp"
#include <string>
#include <cassert> // Add missing include for assert

// for now just going to test my utility functions
int main() {
    TunableEval randomEvals = initializeRandomTunableEval();
    std::string randomChromosone = convertEvalToChromosone(randomEvals);
    TunableEval randomEvalsClone = convertChromosoneToEval(randomChromosone);

    // assert to see if they are the same
    assert(randomEvals.pawn.endGame == randomEvalsClone.pawn.endGame / 10); 
    assert(randomEvals.kingPressureScore.endGame == randomEvalsClone.kingPressureScore.endGame);
    return 0;
}
