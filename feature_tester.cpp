#include <iostream>
#include <string>
#include "features.hpp"
#include "chess.hpp"
#include "feature_extractor.hpp"

Features testposition1 = {
    .fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", // Default FEN string for the initial chess position
    .wpawns = {chess::SQ_A2, chess::SQ_B2, chess::SQ_C2, chess::SQ_D2, chess::SQ_E2, chess::SQ_F2, chess::SQ_G2, chess::SQ_H2}, 
    .bpawns = {chess::SQ_A7, chess::SQ_B7, chess::SQ_C7, chess::SQ_D7, chess::SQ_E7, chess::SQ_F7, chess::SQ_G7, chess::SQ_H7},
    .wknights = {chess::SQ_B1, chess::SQ_G1},
    .bknights = {chess::SQ_B8, chess::SQ_G8},
    .wbishops = {chess::SQ_C1, chess::SQ_F1},
    .bbishops = {chess::SQ_C8, chess::SQ_F8},
    .wrooks = {chess::SQ_A1, chess::SQ_H1},
    .brooks = {chess::SQ_A8, chess::SQ_H8},
    .wqueen = {chess::SQ_D1},
    .bqueen = {chess::SQ_D8},
    .wking = {chess::SQ_E1},
    .bking = {chess::SQ_E8},
    .passedPawns = 0, // No passed pawns initially
    .doubledPawns = 0, // No doubled pawns initially
    .isolatedPawns = 0, // No isolated pawns initially
    .backwardPawns = 0, // No backward pawns initially
    .weakSquares = 0, // No weak squares initially
    .passedPawnEnemyKingSquare = 0, // No passed pawn in enemy king's square initially
    .knightOutposts = 0, // No knight outposts initially
    .bishopMobility = 0, // No bishop mobility initially
    .bishopPair = 1, // Both bishops initially
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

Features testposition2 = {
    .fen = "8/8/4k3/8/4K3/8/8/8 w - - 0 1", // FEN string for two lonely kings in the center
    .wpawns = {}, // No white pawns
    .bpawns = {}, // No black pawns
    .wknights = {}, // No white knights
    .bknights = {}, // No black knights
    .wbishops = {}, // No white bishops
    .bbishops = {}, // No black bishops
    .wrooks = {}, // No white rooks
    .brooks = {}, // No black rooks
    .wqueen = {}, // No white queen
    .bqueen = {}, // No black queen
    .wking = {chess::SQ_E4}, // White king on e1
    .bking = {chess::SQ_E6}, // Black king on e8
    .passedPawns = 0, // No passed pawns initially
    .doubledPawns = 0, // No doubled pawns initially
    .isolatedPawns = 0, // No isolated pawns initially
    .backwardPawns = 0, // No backward pawns initially
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


bool compareFeatures(const Features& extractedFeatures, const Features& testPosition) {
    bool isSame = true;

    // Compare the FEN strings
    if (extractedFeatures.fen != testPosition.fen) {
        std::cout << "Different FEN strings." << std::endl;
        isSame = false;
    }

    // Compare the pawn positions
    if (extractedFeatures.wpawns != testPosition.wpawns) {
        std::cout << "White pawn positions differ." << std::endl;
        isSame = false;
    }
    if (extractedFeatures.bpawns != testPosition.bpawns) {
        std::cout << "Black pawn positions differ." << std::endl;
        isSame = false;
    }

    // Compare the knight positions
    if (extractedFeatures.wknights != testPosition.wknights) {
        std::cout << "White knight positions differ." << std::endl;
        isSame = false;
    }
    if (extractedFeatures.bknights != testPosition.bknights) {
        std::cout << "Black knight positions differ." << std::endl;
        isSame = false;
    }

    // Compare the bishop positions
    if (extractedFeatures.wbishops != testPosition.wbishops) {
        std::cout << "White bishop positions differ." << std::endl;
        isSame = false;
    }
    if (extractedFeatures.bbishops != testPosition.bbishops) {
        std::cout << "Black bishop positions differ." << std::endl;
        isSame = false;
    }

    // Compare the rook positions
    if (extractedFeatures.wrooks != testPosition.wrooks) {
        std::cout << "White rook positions differ." << std::endl;
        isSame = false;
    }
    if (extractedFeatures.brooks != testPosition.brooks) {
        std::cout << "Black rook positions differ." << std::endl;
        isSame = false;
    }

    // Compare the queen positions
    if (extractedFeatures.wqueen != testPosition.wqueen) {
        std::cout << "White queen positions differ." << std::endl;
        isSame = false;
    }
    if (extractedFeatures.bqueen != testPosition.bqueen) {
        std::cout << "Black queen positions differ." << std::endl;
        isSame = false;
    }

    // Compare the king positions
    if (extractedFeatures.wking != testPosition.wking) {
        std::cout << "White king positions differ." << std::endl;
        isSame = false;
    }
    if (extractedFeatures.bking != testPosition.bking) {
        std::cout << "Black king positions differ." << std::endl;
        isSame = false;
    }

    // Add similar print statements for bishops, rooks, queens, kings

    // Compare other features
    if (extractedFeatures.passedPawns != testPosition.passedPawns) {
        std::cout << "Passed pawns differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.backwardPawns != testPosition.backwardPawns) {
        std::cout << "Backward pawns differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.weakSquares != testPosition.weakSquares) {
        std::cout << "Weak squares differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.passedPawnEnemyKingSquare != testPosition.passedPawnEnemyKingSquare) {
        std::cout << "Passed pawn enemy king square differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.knightOutposts != testPosition.knightOutposts) {
        std::cout << "Knight outposts differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.bishopMobility != testPosition.bishopMobility) {
        std::cout << "Bishop mobility differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.bishopPair != testPosition.bishopPair) {
        std::cout << "Bishop pair differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.rookAttackKingFile != testPosition.rookAttackKingFile) {
        std::cout << "Rook attack king file differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.rookAttackKingAdjFile != testPosition.rookAttackKingAdjFile) {
        std::cout << "Rook attack king adjacent file differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.rook7thRank != testPosition.rook7thRank) {
        std::cout << "Rook 7th rank differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.rookConnected != testPosition.rookConnected) {
        std::cout << "Rook connected differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.rookMobility != testPosition.rookMobility) {
        std::cout << "Rook mobility differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.rookBehindPassedPawn != testPosition.rookBehindPassedPawn) {
        std::cout << "Rook behind passed pawn differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.rookOpenFile != testPosition.rookOpenFile) {
        std::cout << "Rook open file differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.rookSemiOpenFile != testPosition.rookSemiOpenFile) {
        std::cout << "Rook semi-open file differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.rookAtckWeakPawnOpenColumn != testPosition.rookAtckWeakPawnOpenColumn) {
        std::cout << "Rook attack weak pawn open column differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.kingFriendlyPawn != testPosition.kingFriendlyPawn) {
        std::cout << "King friendly pawn differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.kingNoEnemyPawnNear != testPosition.kingNoEnemyPawnNear) {
        std::cout << "King no enemy pawn near differ." << std::endl;
        isSame = false;
    }

    if (extractedFeatures.kingPressureScore != testPosition.kingPressureScore) {
        std::cout << "King pressure score differ." << std::endl;
        isSame = false;
    }

    
    // Add similar print statements for other features like doubledPawns, isolatedPawns, etc.

    return isSame;
}


int main() {
    // Test position 1
    Board board1 = Board(testposition1.fen);
    FeatureExtractor extractor1 = FeatureExtractor(board1);
    extractor1.extract();
    Features extractedFeatures1 = extractor1.getFeatures();
    if (compareFeatures(extractedFeatures1, testposition1)) {
        std::cout << "Test position 1: Features match!" << std::endl;
    } else {
        std::cout << "Test position 1: Features do not match!" << std::endl;
    }

    // Test position 2
    Board board2 = Board(testposition2.fen);
    FeatureExtractor extractor2 = FeatureExtractor(board2);
    extractor2.extract();
    Features extractedFeatures2 = extractor2.getFeatures();
    if (compareFeatures(extractedFeatures2, testposition2)) {
        std::cout << "Test position 2: Features match!" << std::endl;
    } else {
        std::cout << "Test position 2: Features do not match!" << std::endl;
    }
    return 0;
}
