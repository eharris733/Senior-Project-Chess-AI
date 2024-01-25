#include <iostream>
#include <string>
#include "features.hpp"
#include "chess.hpp"
#include "feature_extractor.hpp"
#include "evaluator.hpp"

Features testposition1 = {
    .fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", // Default FEN string for the initial chess position
    .passedPawns = 0, // No passed pawns initially
    .doubledPawns = 0, // No doubled pawns initially
    .isolatedPawns = 0, // No isolated pawns initially
    .weakPawns = 0, // No backward pawns initially
    .weakSquares = 0, // No weak squares initially
    .passedPawnEnemyKingSquare = 0, // No passed pawn in enemy king's square initially
    .knightOutposts = 0, // No knight outposts initially
    .bishopMobility = 0, // No bishop mobility initially
    .bishopPair = 0, // Both bishops initially for both sides
    .rookAttackKingFile = 0, // No rook attacking king's file initially
    .rookAttackKingAdjFile = 0, // No rook attacking adjacent to king's file initially
    .rook7thRank = 0, // No rook on 7th rank initially
    .rookConnected = 0, // No connected rooks initially
    .rookMobility = 0, // No rook mobility initially
    .rookBehindPassedPawn = 0, // No rook behind passed pawn initially
    .rookOpenFile = 0, // No rook on open file initially
    .rookSemiOpenFile = 0, // No rook on semi-open file initially
    .rookAtckWeakPawnOpenColumn = 0, // No rook attacking weak pawn on open column initially
    .kingFriendlyPawn = 0, // three friendly pawns nearby initially
    .kingNoEnemyPawnNear = 0, // offset each other initially
    .kingPressureScore = 0.0f // pressure is completely equal initially
};

Features testposition2 = {
    .fen = "8/8/4k3/8/4K3/8/8/8 w - - 0 1", // FEN string for two lonely kings in the center
    .passedPawns = 0, // No passed pawns initially
    .doubledPawns = 0, // No doubled pawns initially
    .isolatedPawns = 0, // No isolated pawns initially
    .weakPawns = 0, // No backward pawns initially
    .weakSquares = 0, // No weak squares initially
    .passedPawnEnemyKingSquare = 0, // No passed pawn in enemy king's square initially
    .knightOutposts = 0, // No knight outposts initially
    .bishopMobility = 0, // No bishop mobility initially
    .bishopPair = 0, // No bishop pair initially
    .rookAttackKingFile = 0, // No rook attacking king's file initially
    .rookAttackKingAdjFile = 0, // No rook attacking adjacent to king's file initially
    .rook7thRank = 0, // No rook on 7th rank initially
    .rookConnected = 0, // No connected rooks initially
    .rookMobility = 0, // No rook mobility initially
    .rookBehindPassedPawn = 0, // No rook behind passed pawn initially
    .rookOpenFile = 0, // No rook on open file initially
    .rookSemiOpenFile = 0, // No rook on semi-open file initially
    .rookAtckWeakPawnOpenColumn = 0, // No rook attacking weak pawn on open column initially
    .kingFriendlyPawn = 0, // No friendly pawns near king initially
    .kingNoEnemyPawnNear = 0, // No enemy pawns near king initially
    .kingPressureScore = 0.0f // No king pressure initially
};

Features testposition3 = {
    .fen = "r1bq1r1k/6pp/p2pBb2/1p1Nn2Q/3NP2P/8/PPP5/1K1R3R b - - 2 20", // A random complicated position from the sicilian
    .passedPawns = 0, // No passed pawns 
    .doubledPawns = 0, // No doubled pawns
    .isolatedPawns = 1, // white has two, black has one
    .weakPawns = 0, // No backward pawns 
    .weakSquares = -1, //black has 5, white has 4
    .passedPawnEnemyKingSquare = 0, // No passed pawns
    .knightOutposts = 0, // each side has one
    .bishopMobility = 2, // whites bishop has 6 open squares, blacks two each have two 
    .bishopPair = -1, // only black has bishop pair
    .rookAttackKingFile = 1, // only white is attacking black king's file
    .rookAttackKingAdjFile = -1, // only black is
    .rook7thRank = 0, // No rook on 7th rank initially
    .rookConnected = 1, // only white is
    .rookMobility = 6, // white has eleven moves, black has 5
    .rookBehindPassedPawn = 0, // No passed pawns
    .rookOpenFile = -1, // Black's rook is on an open file
    .rookSemiOpenFile = 1, //white's rook is
    .rookAtckWeakPawnOpenColumn = 1, //whites rook is eyeing an isolated pawn
    .kingFriendlyPawn = 3, // white has an extra friendly pawn nearby
    .kingNoEnemyPawnNear = 0, // No enemy pawns near king 
    .kingPressureScore = 0.0f // should be positive, white has more pressure
};


bool compareFeatures(const Features& extractedFeatures, const Features& testPosition) {
    bool isSame = true;

    // Compare the FEN strings
    if (extractedFeatures.fen != testPosition.fen) {
        std::cout << "Different FEN strings." << std::endl;
        std::cout << "Expected: " << testPosition.fen << std::endl;
        std::cout << "Got: " << extractedFeatures.fen << std::endl;
        isSame = false;
    }

    // Compare more involved features
    if (extractedFeatures.passedPawns != testPosition.passedPawns) {
        std::cout << "Passed pawns differ." << std::endl;
        cout << "Expected: " << testPosition.passedPawns << endl;
        cout << "Got: " << extractedFeatures.passedPawns << endl;
        isSame = false;
    }

    if (extractedFeatures.weakPawns != testPosition.weakPawns) {
        std::cout << "Backward pawns differ." << std::endl;
        cout << "Expected: " << testPosition.weakPawns << endl;
        cout << "Got: " << extractedFeatures.weakPawns << endl;
        isSame = false;
    }

    if (extractedFeatures.weakSquares != testPosition.weakSquares) {
        std::cout << "Weak squares differ." << std::endl;
        cout << "Expected: " << testPosition.weakSquares << endl;
        cout << "Got: " << extractedFeatures.weakSquares << endl;
        isSame = false;
    }

    if (extractedFeatures.passedPawnEnemyKingSquare != testPosition.passedPawnEnemyKingSquare) {
        std::cout << "Passed pawn enemy king square differ." << std::endl;
        cout << "Expected: " << testPosition.passedPawnEnemyKingSquare << endl;
        cout << "Got: " << extractedFeatures.passedPawnEnemyKingSquare << endl;
        isSame = false;
    }

    if (extractedFeatures.knightOutposts != testPosition.knightOutposts) {
        std::cout << "Knight outposts differ." << std::endl;
        cout << "Expected: " << testPosition.knightOutposts << endl;
        cout << "Got: " << extractedFeatures.knightOutposts << endl;
        isSame = false;
    }

    if (extractedFeatures.bishopMobility != testPosition.bishopMobility) {
        std::cout << "Bishop mobility differ." << std::endl;
        cout << "Expected: " << testPosition.bishopMobility << endl;
        cout << "Got: " << extractedFeatures.bishopMobility << endl;
        isSame = false;
    }

    if (extractedFeatures.bishopPair != testPosition.bishopPair) {
        std::cout << "Bishop pair differ." << std::endl;
        cout << "Expected: " << testPosition.bishopPair << endl;
        cout << "Got: " << extractedFeatures.bishopPair << endl;
        isSame = false;
    }

    if (extractedFeatures.rookAttackKingFile != testPosition.rookAttackKingFile) {
        std::cout << "Rook attack king file differ." << std::endl;
        cout << "Expected: " << testPosition.rookAttackKingFile << endl;
        cout << "Got: " << extractedFeatures.rookAttackKingFile << endl;
        isSame = false;
    }

    if (extractedFeatures.rookAttackKingAdjFile != testPosition.rookAttackKingAdjFile) {
        std::cout << "Rook attack king adjacent file differ." << std::endl;
        cout << "Expected: " << testPosition.rookAttackKingAdjFile << endl;
        cout << "Got: " << extractedFeatures.rookAttackKingAdjFile << endl;
        isSame = false;
    }

    if (extractedFeatures.rook7thRank != testPosition.rook7thRank) {
        std::cout << "Rook 7th rank differ." << std::endl;
        cout << "Expected: " << testPosition.rook7thRank << endl;
        cout << "Got: " << extractedFeatures.rook7thRank << endl;
        isSame = false;
    }

    if (extractedFeatures.rookConnected != testPosition.rookConnected) {
        std::cout << "Rook connected differ." << std::endl;
        cout << "Expected: " << testPosition.rookConnected << endl;
        cout << "Got: " << extractedFeatures.rookConnected << endl;
        isSame = false;
    }

    if (extractedFeatures.rookMobility != testPosition.rookMobility) {
        std::cout << "Rook mobility differ." << std::endl;
        cout << "Expected: " << testPosition.rookMobility << endl;
        cout << "Got: " << extractedFeatures.rookMobility << endl;
        isSame = false;
    }

    if (extractedFeatures.rookBehindPassedPawn != testPosition.rookBehindPassedPawn) {
        std::cout << "Rook behind passed pawn differ." << std::endl;
        cout << "Expected: " << testPosition.rookBehindPassedPawn << endl;
        cout << "Got: " << extractedFeatures.rookBehindPassedPawn << endl;
        isSame = false;
    }

    if (extractedFeatures.rookOpenFile != testPosition.rookOpenFile) {
        std::cout << "Rook open file differ." << std::endl;
        cout << "Expected: " << testPosition.rookOpenFile << endl;
        cout << "Got: " << extractedFeatures.rookOpenFile << endl;
        isSame = false;
    }

    if (extractedFeatures.rookSemiOpenFile != testPosition.rookSemiOpenFile) {
        std::cout << "Rook semi-open file differ." << std::endl;
        cout << "Expected: " << testPosition.rookSemiOpenFile << endl;
        cout << "Got: " << extractedFeatures.rookSemiOpenFile << endl;
        isSame = false;
    }

    if (extractedFeatures.rookAtckWeakPawnOpenColumn != testPosition.rookAtckWeakPawnOpenColumn) {
        std::cout << "Rook attack weak pawn open column differ." << std::endl;
        cout << "Expected: " << testPosition.rookAtckWeakPawnOpenColumn << endl;
        cout << "Got: " << extractedFeatures.rookAtckWeakPawnOpenColumn << endl;
        isSame = false;
    }

    if (extractedFeatures.kingFriendlyPawn != testPosition.kingFriendlyPawn) {
        std::cout << "King friendly pawn differ." << std::endl;
        cout << "Expected: " << testPosition.kingFriendlyPawn << endl;
        cout << "Got: " << extractedFeatures.kingFriendlyPawn << endl;
        isSame = false;
    }

    if (extractedFeatures.kingNoEnemyPawnNear != testPosition.kingNoEnemyPawnNear) {
        std::cout << "King no enemy pawn near differ." << std::endl;
        cout << "Expected: " << testPosition.kingNoEnemyPawnNear << endl;
        cout << "Got: " << extractedFeatures.kingNoEnemyPawnNear << endl;
        isSame = false;
    }

    if (extractedFeatures.kingPressureScore != testPosition.kingPressureScore) {
        std::cout << "King pressure score differ." << std::endl;
        cout << "Expected: " << testPosition.kingPressureScore << endl;
        cout << "Got: " << extractedFeatures.kingPressureScore << endl;
        isSame = false;
    }


    
    // Add similar print statements for other features like doubledPawns, isolatedPawns, etc.

    return isSame;
}


int main() {
    // Test position 1
    Board board1 = Board(testposition1.fen);
    FeatureExtractor extractor1 = FeatureExtractor(board1);
    Evaluator evaluator1 = Evaluator(board1);
    extractor1.extract();
    std::cout << "evaluation for position 1: " << evaluator1.evaluate(0) << std::endl;
    Features extractedFeatures1 = extractor1.getFeatures();
    if (compareFeatures(extractedFeatures1, testposition1)) {
        std::cout << "Test position 1: Features match!" << std::endl;
    } else {
        std::cout << "Test position 1: Features do not match!" << std::endl;
    }

    // Test position 2
    Board board2 = Board(testposition2.fen);
    FeatureExtractor extractor2 = FeatureExtractor(board2);
    Evaluator evaluator2 = Evaluator(board2);
    extractor2.extract();
    std::cout << "evaluation for position 2: " << evaluator2.evaluate(0) << std::endl;
    Features extractedFeatures2 = extractor2.getFeatures();
    if (compareFeatures(extractedFeatures2, testposition2)) {
        std::cout << "Test position 2: Features match!" << std::endl;
    } else {
        std::cout << "Test position 2: Features do not match!" << std::endl;
    }

    // Test position 3
    Board board3 = Board(testposition3.fen);
    FeatureExtractor extractor3 = FeatureExtractor(board3);
    Evaluator evaluator3 = Evaluator(board3);
    extractor3.extract();
    Features extractedFeatures3 = extractor3.getFeatures();
    std::cout << "evaluation for position 3: " << evaluator3.evaluate(0) << std::endl;
    if (compareFeatures(extractedFeatures3, testposition3)) {
        std::cout << "Test position 3: Features match!" << std::endl;
    } else {
        std::cout << "Test position 3: Features do not match!" << std::endl;
    }



   
}
