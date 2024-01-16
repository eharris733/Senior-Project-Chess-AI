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
    // Compare the FEN strings
    if (extractedFeatures.fen != testPosition.fen) {
        return false;
    }

    // Compare the pawn positions
    if (extractedFeatures.wpawns != testPosition.wpawns ||
        extractedFeatures.bpawns != testPosition.bpawns) {
        return false;
    }

    // Compare the knight positions
    if (extractedFeatures.wknights != testPosition.wknights ||
        extractedFeatures.bknights != testPosition.bknights) {
        return false;
    }

    // Compare the bishop positions
    if (extractedFeatures.wbishops != testPosition.wbishops ||
        extractedFeatures.bbishops != testPosition.bbishops) {
        return false;
    }

    // Compare the rook positions
    if (extractedFeatures.wrooks != testPosition.wrooks ||
        extractedFeatures.brooks != testPosition.brooks) {
        return false;
    }

    // Compare the queen positions
    if (extractedFeatures.wqueen != testPosition.wqueen ||
        extractedFeatures.bqueen != testPosition.bqueen) {
        return false;
    }

    // Compare the king positions
    if (extractedFeatures.wking != testPosition.wking ||
        extractedFeatures.bking != testPosition.bking) {
        return false;
    }

    // Compare the remaining features
    if (extractedFeatures.passedPawns != testPosition.passedPawns ||
        extractedFeatures.doubledPawns != testPosition.doubledPawns ||
        extractedFeatures.isolatedPawns != testPosition.isolatedPawns ||
        extractedFeatures.backwardPawns != testPosition.backwardPawns ||
        extractedFeatures.weakSquares != testPosition.weakSquares ||
        extractedFeatures.passedPawnEnemyKingSquare != testPosition.passedPawnEnemyKingSquare ||
        extractedFeatures.knightOutposts != testPosition.knightOutposts ||
        extractedFeatures.bishopMobility != testPosition.bishopMobility ||
        extractedFeatures.bishopPair != testPosition.bishopPair ||
        extractedFeatures.rookAttackKingFile != testPosition.rookAttackKingFile ||
        extractedFeatures.rookAttackKingAdjFile != testPosition.rookAttackKingAdjFile ||
        extractedFeatures.rook7thRank != testPosition.rook7thRank ||
        extractedFeatures.rookConnected != testPosition.rookConnected ||
        extractedFeatures.rookMobility != testPosition.rookMobility ||
        extractedFeatures.rookBehindPassedPawn != testPosition.rookBehindPassedPawn ||
        extractedFeatures.rookOpenFile != testPosition.rookOpenFile ||
        extractedFeatures.rookSemiOpenFile != testPosition.rookSemiOpenFile ||
        extractedFeatures.rookAtckWeakPawnOpenColumn != testPosition.rookAtckWeakPawnOpenColumn ||
        extractedFeatures.kingFriendlyPawn != testPosition.kingFriendlyPawn ||
        extractedFeatures.kingNoEnemyPawnNear != testPosition.kingNoEnemyPawnNear ||
        extractedFeatures.kingPressureScore != testPosition.kingPressureScore) {
        return false;
    }

    return true;
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
