#include "chess.hpp"
#include "evaluator.hpp"
#include "baselines.hpp"
#include <cstdlib>
#include <string>

 using namespace chess;

// this function should be an exact copy of the evaluation function in the evaluator class
// only difference is this one tracks everything as it goes, so one can debug. 
std::string testEvaluation(Board& board, bool lazy = false, TunableEval featureWeights = baseEval){
    //bitboards we need
        std::string features = "";
        float gamePhase = 0;

        Bitboard pieces = board.occ();
        Bitboard wPawns = 0;
        Bitboard bPawns = 0;
        Bitboard wKnights = 0;
        Bitboard bKnights = 0;
        Bitboard wBishops = 0;
        Bitboard bBishops = 0;
        Bitboard wRooks = 0;
        Bitboard bRooks = 0;
        Bitboard wQueens = 0;
        Bitboard bQueens = 0;
        Bitboard wKings = 0; // these are bitboards even though there can only be one
        Bitboard bKings = 0;

        //useful bitboards for pawn structure and the like
        Bitboard wPawnAttacks = Bitboard();
        Bitboard bPawnAttacks = Bitboard();

        Bitboard wKnightAttacks = Bitboard();
        Bitboard bKnightAttacks = Bitboard();

        Bitboard wBishopAttacks = Bitboard();
        Bitboard bBishopAttacks = 0;

        Bitboard wRookAttacks = 0;
        Bitboard bRookAttacks = 0;

        Bitboard wQueenAttacks = 0;
        Bitboard bQueenAttacks = 0;

        
        // variables we need throughout the function
        float score = 0;
        int taperedEndgameScore = 0; 
        int mgscore = 0;
        int egscore = 0;


        // main eval loop
        for (int i = 0; i < 64; i++){
            Square sq = int_to_square(i);
            int sqi = i; // for the piece tables
            Piece piece = board.at(sq);

            // switch to handle each type of piece
            switch(piece){
                case Piece::NONE:
                    break;
                case Piece::WHITEPAWN:
                    wPawns |= square_to_bitmap(sq);
                    wPawnAttacks |= attacks::pawn(Color::WHITE, sq);
                    taperedEndgameScore += 1;
                    mgscore += featureWeights.pawn.middleGame;
                    egscore += featureWeights.pawn.endGame;
                    break;
                case Piece::BLACKPAWN:
                    bPawns |= square_to_bitmap(sq);
                    bPawnAttacks |= attacks::pawn(Color::BLACK, sq);
                    taperedEndgameScore += 1;
                    mgscore -= featureWeights.pawn.middleGame;
                    egscore -= featureWeights.pawn.endGame;
                    break;
                case Piece::WHITEKNIGHT:
                    wKnights |= square_to_bitmap(sq);
                    taperedEndgameScore += 3;
                    wKnightAttacks |= attacks::knight(sq);
                    mgscore += featureWeights.knight.middleGame;
                    egscore += featureWeights.knight.endGame;
                    break;
                case Piece::BLACKKNIGHT:
                    bKnights |= square_to_bitmap(sq);
                    taperedEndgameScore += 3;
                    bKnightAttacks |= attacks::knight(sq);
                    mgscore -= featureWeights.knight.middleGame;
                    egscore -= featureWeights.knight.endGame;
                    break;
                case Piece::WHITEBISHOP:
                    wBishops |= square_to_bitmap(sq);
                    taperedEndgameScore += 3;
                    wBishopAttacks |= attacks::bishop(sq, pieces);
                    mgscore += featureWeights.bishop.middleGame;
                    egscore += featureWeights.bishop.endGame;
                    break;
                case Piece::BLACKBISHOP:
                    bBishops |= square_to_bitmap(sq);
                    taperedEndgameScore += 3;
                    bBishopAttacks |= attacks::bishop(sq, pieces);
                    mgscore -= featureWeights.bishop.middleGame;
                    egscore -= featureWeights.bishop.endGame;
                    break;
                case Piece::WHITEROOK:
                    wRooks |= square_to_bitmap(sq);
                    taperedEndgameScore += 5;
                    wRookAttacks |= attacks::rook(sq, pieces);
                    mgscore += featureWeights.rook.middleGame;
                    egscore += featureWeights.rook.endGame;
                    break;
                case Piece::BLACKROOK:
                    bRooks |= square_to_bitmap(sq);
                    taperedEndgameScore += 5;
                    bRookAttacks |= attacks::rook(sq, pieces);
                    mgscore -= featureWeights.rook.middleGame;
                    egscore -= featureWeights.rook.endGame;
                    break;
                case Piece::WHITEQUEEN:
                    wQueens |= square_to_bitmap(sq);
                    taperedEndgameScore += 9;
                    wQueenAttacks |= attacks::queen(sq, pieces);
                    mgscore += featureWeights.queen.middleGame;
                    egscore += featureWeights.queen.endGame;
                    break;
                case Piece::BLACKQUEEN:
                    bQueens |= square_to_bitmap(sq);
                    taperedEndgameScore += 9;
                    bQueenAttacks |= attacks::queen(sq, pieces);
                    mgscore -= featureWeights.queen.middleGame;
                    egscore -= featureWeights.queen.endGame;
                    break;
                case Piece::WHITEKING:
                    wKings |= square_to_bitmap(sq);
                    break;
                case Piece::BLACKKING:
                    bKings |= square_to_bitmap(sq);
                    break;
            }
        }


        Bitboard allBPieces = bPawns | bKnights | bBishops | bRooks | bQueens | bKings;
        Bitboard allWPieces = wPawns | wKnights | wBishops | wRooks | wQueens | wKings;
        Bitboard allBPiecesXQueens = allBPieces & ~bQueens;
        Bitboard allWPiecesXQueens = allWPieces & ~wQueens;
        Bitboard allWPiecesXQR = allWPieces & ~(wQueens | wRooks);
        Bitboard allBPiecesXQR = allBPieces & ~(bQueens | bRooks);
        
        // various x-rays and stops for pieces that can't jump
        wBishopAttacks &= ~allWPieces & ~allBPiecesXQR;
        bBishopAttacks &= ~allBPieces & ~allWPiecesXQR;
        wRookAttacks &= ~allWPieces & ~allBPiecesXQueens;
        bRookAttacks &= ~allBPieces & ~allWPiecesXQueens;
        wQueenAttacks &= ~allWPieces;
        bQueenAttacks &= ~allBPieces;

        


        // Calculate the game phase dynamically based on the endgame score
        gamePhase = std::max(0.0f, std::min(1.0f, (taperedEndgameScore - 24) / 24.0f)); // Ensure the game phase is between 0 and 1
        
        float mgWeight = gamePhase;
        float egWeight = 1 - gamePhase;
        

        features += "Game Phase: " + std::to_string(gamePhase) + "\n";
        features +=  "Middle Game Weight: " + std::to_string(mgWeight) + "\n";
        features +=  "End Game Weight: " + std::to_string(egWeight) + "\n";


        // Combine middle game and end game scores based on the current game phase
        score = mgscore * mgWeight + egscore * egWeight;

        features += "Material Score: " + std::to_string(score) + "\n";

        // if lazy evaluation is enabled, return the score here
        // we can also be lazy in evaluating completely winning positions not in the endgame
        if(lazy || ((abs(score) > 500 && gamePhase > .2))){
            features += "Lazy Evaluation: " + std::to_string(score) + "\n";
            return features;
        }

        // extract the rest of the features from the board

        // passed pawns (tested and working)
        Bitboard whitePassedPawns = detectPassedPawns(Color::WHITE, wPawns, bPawns);
        Bitboard blackPassedPawns = detectPassedPawns(Color::BLACK, bPawns, wPawns);

        features += "White Passed Pawns: " + std::to_string(builtin::popcount(whitePassedPawns)) + "\n";
        features += "Black Passed Pawns: " + std::to_string(builtin::popcount(blackPassedPawns)) + "\n";

        score += (builtin::popcount(whitePassedPawns) - builtin::popcount(blackPassedPawns)) * (featureWeights.passedPawn.middleGame * mgWeight + featureWeights.passedPawn.endGame * egWeight);

        features += "Score after evaluation passed Pawns: " + std::to_string(score) + "\n";

        // doubled pawns (tested and working)
        // adjusting the score negatively for doubled pawns
        features += "White Doubled Pawns: " + std::to_string(detectDoubledPawns(Color::WHITE, wPawns, bPawns)) + "\n";
        features += "Black Doubled Pawns: " + std::to_string(detectDoubledPawns(Color::BLACK, bPawns, wPawns)) + "\n";
        score -= (detectDoubledPawns(Color::BLACK, wPawns, bPawns) - detectDoubledPawns(Color::WHITE, bPawns, wPawns)) * (featureWeights.doubledPawn.middleGame * mgWeight + featureWeights.doubledPawn.endGame * egWeight);
        features += "Score after evaluation doubled Pawns: " + std::to_string(score) + "\n";

        Bitboard isolatedWhitePawns = detectIsolatedPawns(wPawns);
        Bitboard isolatedBlackPawns = detectIsolatedPawns(bPawns);
        features += "White Isolated Pawns: " + std::to_string(builtin::popcount(isolatedWhitePawns)) + "\n";
        features += "Black Isolated Pawns: " + std::to_string(builtin::popcount(isolatedBlackPawns)) + "\n";
        // isolated pawns (tested and working)
        // adjusting the score negatively for isolated pawns
        score -= (builtin::popcount(isolatedWhitePawns) - builtin::popcount(isolatedBlackPawns)) * (featureWeights.isolatedPawn.middleGame * mgWeight + featureWeights.isolatedPawn.endGame * egWeight);
        features += "Score after evaluation isolated Pawns: " + std::to_string(score) + "\n";

        // weak pawns (finally working)
        // adjusting the score negatively for weak pawns
        Bitboard weakWhitePawns = wBackward(wPawns, bPawns);
        Bitboard weakBlackPawns = bBackward(bPawns, wPawns);
        features += "White Weak Pawns: " + std::to_string(builtin::popcount(weakWhitePawns)) + "\n";
        features += "Black Weak Pawns: " + std::to_string(builtin::popcount(weakBlackPawns)) + "\n";

        
        // exclude passers and isolated pawns from weak pawns
        weakWhitePawns &= ~whitePassedPawns & ~isolatedWhitePawns;
        weakBlackPawns &= ~blackPassedPawns & ~isolatedBlackPawns;

        score -= (builtin::popcount(weakBlackPawns) - builtin::popcount(weakWhitePawns)) * (featureWeights.weakPawn.middleGame * mgWeight + featureWeights.weakPawn.endGame * egWeight);
        features += "Score after evaluation weak Pawns: " + std::to_string(score) + "\n";

        // central pawns (not tested)
        features += "White Central Pawns: " + std::to_string(builtin::popcount(detectCentralPawns(wPawns))) + "\n";
        features += "Black Central Pawns: " + std::to_string(builtin::popcount(detectCentralPawns(bPawns))) + "\n";
        score += (builtin::popcount(detectCentralPawns(wPawns)) - builtin::popcount(detectCentralPawns(bPawns))) * (featureWeights.centralPawn.middleGame * mgWeight + featureWeights.centralPawn.endGame * egWeight);
        features += "Score after evaluation central Pawns: " + std::to_string(score) + "\n";

        // weak squares (working finally)
        // adjusting the score negatively for weak squares
        Bitboard weakWhiteSquares = detectWeakSquares(Color::WHITE, wPawns);
        Bitboard weakBlackSquares = detectWeakSquares(Color::BLACK, bPawns);
        features += "White Weak Squares: " + std::to_string(builtin::popcount(weakWhiteSquares)) + "\n";
        features += "Black Weak Squares: " + std::to_string(builtin::popcount(weakBlackSquares)) + "\n";
        score -= (builtin::popcount(weakBlackSquares) - builtin::popcount(weakWhiteSquares)) * (featureWeights.weakSquare.middleGame * mgWeight + featureWeights.weakSquare.endGame * egWeight);
        features += "Score after evaluation weak Squares: " + std::to_string(score) + "\n";


        features += "white rule of the square: " + std::to_string(ruleOfTheSquare(Color::WHITE, blackPassedPawns, wKings)) + "\n";
        features += "black rule of the square: " + std::to_string(ruleOfTheSquare(Color::BLACK, whitePassedPawns, bKings)) + "\n";
        // rule of the square (tested and working)
        score += (ruleOfTheSquare(Color::WHITE, blackPassedPawns, wKings) - ruleOfTheSquare(Color::BLACK, whitePassedPawns, bKings)) * (featureWeights.passedPawnEnemyKingSquare.middleGame * mgWeight + featureWeights.passedPawnEnemyKingSquare.endGame * egWeight);
        features += "Score after evaluation rule of the square: " + std::to_string(score) + "\n";


        features += "white knight outposts: " + std::to_string(builtin::popcount(knightOutposts(weakBlackSquares, wKnights, wPawnAttacks))) + "\n";
        features += "black knight outposts: " + std::to_string(builtin::popcount(knightOutposts(weakWhiteSquares, bKnights, bPawnAttacks))) + "\n";
        // knight outposts (tested like 90% sure it works)
        score += (builtin::popcount(knightOutposts(weakBlackSquares, wKnights, wPawnAttacks)) - builtin::popcount(knightOutposts(weakWhiteSquares, bKnights, bPawnAttacks))) * (featureWeights.knightOutposts.middleGame * mgWeight + featureWeights.knightOutposts.endGame * egWeight);
        features += "Score after evaluation knight outposts: " + std::to_string(score) + "\n";


        features += "white knight mobility: " + std::to_string(knightMobility(wKnightAttacks)) + "\n";
        features += "black knight mobility: " + std::to_string(knightMobility(bKnightAttacks)) + "\n";
        // knight mobility (not tested)
        score += (knightMobility(wKnightAttacks) - knightMobility(bKnightAttacks)) * (featureWeights.knightMobility.middleGame * mgWeight + featureWeights.knightMobility.endGame * egWeight);
        features += "Score after evaluation knight mobility: " + std::to_string(score) + "\n";

        features += "white bishop mobility: " + std::to_string(bishopMobility(wBishopAttacks)) + "\n";
        features += "black bishop mobility: " + std::to_string(bishopMobility(bBishopAttacks)) + "\n";
        // bishop mobility (tested)
        score += (bishopMobility(wBishopAttacks) - bishopMobility(bBishopAttacks)) * (featureWeights.bishopMobility.middleGame * mgWeight + featureWeights.bishopMobility.endGame * egWeight);
        features += "Score after evaluation bishop mobility: " + std::to_string(score) + "\n";

        features += "white bishop pair: " + std::to_string(bishopPair(wBishops)) + "\n";
        features += "black bishop pair: " + std::to_string(bishopPair(bBishops)) + "\n";
        // bishop pair (tested and working)
        score += (bishopPair(wBishops) - bishopPair(bBishops)) * (featureWeights.bishopPair.middleGame * mgWeight + featureWeights.bishopPair.endGame * egWeight);
        features += "Score after evaluation bishop pair: " + std::to_string(score) + "\n";

        // rook attack king file (tested and fixed)
        features += "white rook attack king file: " + std::to_string(rookAttackKingFile(Color::WHITE, wRooks, bKings)) + "\n";
        features += "black rook attack king file: " + std::to_string(rookAttackKingFile(Color::BLACK, bRooks, wKings)) + "\n";
        score += (rookAttackKingFile(Color::WHITE, wRooks, bKings) - rookAttackKingFile(Color::BLACK, bRooks, wKings)) * (featureWeights.rookAttackKingFile.middleGame * mgWeight + featureWeights.rookAttackKingFile.endGame * egWeight);
        features += "Score after evaluation rook attack king file: " + std::to_string(score) + "\n";

        // rook attack king adjacent file (tested and fixed)
        features += "white rook attack king adjacent file: " + std::to_string(rookAttackKingAdjFile(Color::WHITE, wRooks, bKings)) + "\n";
        features += "black rook attack king adjacent file: " + std::to_string(rookAttackKingAdjFile(Color::BLACK, bRooks, wKings)) + "\n";
        score += (rookAttackKingAdjFile(Color::WHITE, wRooks, bKings) - rookAttackKingAdjFile(Color::BLACK, bRooks, wKings)) * (featureWeights.rookAttackKingAdjFile.middleGame * mgWeight + featureWeights.rookAttackKingAdjFile.endGame * egWeight);
        features += "Score after evaluation rook attack king adjacent file: " + std::to_string(score) + "\n";

        // rook on 7th rank (tested and working)
        features += "white rook 7th rank: " + std::to_string(rookSeventhRank(Color::WHITE, wRooks)) + "\n";
        features += "black rook 7th rank: " + std::to_string(rookSeventhRank(Color::BLACK, bRooks)) + "\n";
        score += (rookSeventhRank(Color::WHITE, wRooks) - rookSeventhRank(Color::BLACK, bRooks)) * (featureWeights.rook7thRank.middleGame * mgWeight + featureWeights.rook7thRank.endGame * egWeight);
        features += "Score after evaluation rook 7th rank: " + std::to_string(score) + "\n";
        

        // rook connected (tested and working)
        features += "white rook connected: " + std::to_string(rookConnected(Color::WHITE, wRooks, pieces)) + "\n";
        features += "black rook connected: " + std::to_string(rookConnected(Color::BLACK, bRooks, pieces)) + "\n";
        score += (rookConnected(Color::WHITE, wRooks, pieces) - rookConnected(Color::BLACK, bRooks, pieces)) * (featureWeights.rookConnected.middleGame * mgWeight + featureWeights.rookConnected.endGame * egWeight);
        features += "Score after evaluation rook connected: " + std::to_string(score) + "\n";

        // rook mobility (tested and working)
        features += "white rook mobility: " + std::to_string(rookMobility(wRookAttacks)) + "\n";
        features += "black rook mobility: " + std::to_string(rookMobility(bRookAttacks)) + "\n";
        score += (rookMobility(wRookAttacks) - rookMobility(bRookAttacks)) * (featureWeights.rookMobility.middleGame * mgWeight + featureWeights.rookMobility.endGame * egWeight);
        features += "Score after evaluation rook mobility: " + std::to_string(score) + "\n";


        // rook behind passed pawn (tested and working) 
        features += "white rook behind passed pawn: " + std::to_string(rookBehindPassedPawn(Color::WHITE, wRooks, whitePassedPawns)) + "\n";
        features += "black rook behind passed pawn: " + std::to_string(rookBehindPassedPawn(Color::BLACK, bRooks, blackPassedPawns)) + "\n";
        score += (rookBehindPassedPawn(Color::WHITE, wRooks, whitePassedPawns) - rookBehindPassedPawn(Color::BLACK, bRooks, blackPassedPawns)) * (featureWeights.rookBehindPassedPawn.middleGame * mgWeight + featureWeights.rookBehindPassedPawn.endGame * egWeight);
        features += "Score after evaluation rook behind passed pawn: " + std::to_string(score) + "\n";
        

        // rook on open file
        Bitboard allPawns = wPawns | bPawns;
        features += "white rook open file: " + std::to_string(rookOpenFile(Color::WHITE, wRooks, allPawns)) + "\n";
        features += "black rook open file: " + std::to_string(rookOpenFile(Color::BLACK, bRooks, allPawns)) + "\n";
        score += (rookOpenFile(Color::WHITE, wRooks, allPawns) - rookOpenFile(Color::BLACK, bRooks, allPawns)) * (featureWeights.rookOpenFile.middleGame * mgWeight + featureWeights.rookOpenFile.endGame * egWeight);
        features += "Score after evaluation rook open file: " + std::to_string(score) + "\n";

        // rook on semi-open file
        features += "white rook semi open file: " + std::to_string(rookSemiOpenFile(Color::WHITE, wRooks, wPawns, bPawns)) + "\n";
        features += "black rook semi open file: " + std::to_string(rookSemiOpenFile(Color::BLACK, bRooks, bPawns, wPawns)) + "\n";
        score += (rookSemiOpenFile(Color::WHITE, wRooks, wPawns, bPawns) - rookSemiOpenFile(Color::BLACK, bRooks, bPawns, wPawns)) * (featureWeights.rookSemiOpenFile.middleGame * mgWeight + featureWeights.rookSemiOpenFile.endGame * egWeight);
        features += "Score after evaluation rook semi open file: " + std::to_string(score) + "\n";

        // rook attack weak pawn on open column
        features += "white rook attack weak pawn open column: " + std::to_string(rookAtckWeakPawnOpenColumn(Color::WHITE, wRooks, weakBlackPawns)) + "\n";
        features += "black rook attack weak pawn open column: " + std::to_string(rookAtckWeakPawnOpenColumn(Color::BLACK, bRooks, weakWhitePawns)) + "\n";
        score += (rookAtckWeakPawnOpenColumn(Color::WHITE, wRooks, weakBlackPawns) - rookAtckWeakPawnOpenColumn(Color::BLACK, bRooks, weakWhitePawns)) * (featureWeights.rookAtckWeakPawnOpenColumn.middleGame * mgWeight + featureWeights.rookAtckWeakPawnOpenColumn.endGame * egWeight);
        features += "Score after evaluation rook attack weak pawn open column: " + std::to_string(score) + "\n";

        // queen mobility (not tested)
        features += "white queen mobility: " + std::to_string(queenMobility(wQueenAttacks)) + "\n";
        features += "black queen mobility: " + std::to_string(queenMobility(bQueenAttacks)) + "\n";
        score += (queenMobility(wQueenAttacks) - queenMobility(bQueenAttacks)) * (featureWeights.queenMobility.middleGame * mgWeight + featureWeights.queenMobility.endGame * egWeight);
        features += "Score after evaluation queen mobility: " + std::to_string(score) + "\n";

        // king friendly pawn
        features += "white king friendly pawn: " + std::to_string(kingFriendlyPawn(wPawns, wKings)) + "\n";
        features += "black king friendly pawn: " + std::to_string(kingFriendlyPawn(bPawns, bKings)) + "\n";
        score += (kingFriendlyPawn(wPawns, wKings) - kingFriendlyPawn(bPawns, bKings)) * (featureWeights.kingFriendlyPawn.middleGame * mgWeight + featureWeights.kingFriendlyPawn.endGame * egWeight);
        features += "Score after evaluation king friendly pawn: " + std::to_string(score) + "\n";
        
        // king no enemy pawn near
        features += "white king no enemy pawn near: " + std::to_string(kingNoEnemyPawnNear(wPawns, bKings)) + "\n";
        features += "black king no enemy pawn near: " + std::to_string(kingNoEnemyPawnNear(bPawns, wKings)) + "\n";
        score += (kingNoEnemyPawnNear(bPawns, wKings) - kingNoEnemyPawnNear(wPawns, bKings)) * (featureWeights.kingNoEnemyPawnNear.middleGame * mgWeight + featureWeights.kingNoEnemyPawnNear.endGame * egWeight);
        features += "Score after evaluation king no enemy pawn near: " + std::to_string(score) + "\n";
        
        Evaluator evaluator = Evaluator(board, featureWeights);
        std::vector<int> kingSafetyTable = evaluator.getKingSafetyTable();
        // revised king pressure scores  (yet to be tested)
        features += "white king pressure: " + std::to_string(kingPressureScore(wKings, bKnightAttacks, bBishopAttacks, bRookAttacks, bQueenAttacks,  Color::WHITE, kingSafetyTable)) + "\n";
        features += "black king pressure: " + std::to_string(kingPressureScore(bKings, wKnightAttacks, wBishopAttacks, wRookAttacks, wQueenAttacks,  Color::BLACK, kingSafetyTable)) + "\n";
        // revised king pressure scores  (yet to be tested)
        score += kingPressureScore(wKings, bKnightAttacks, bBishopAttacks, bRookAttacks, bQueenAttacks, Color::WHITE, kingSafetyTable);
        score -= kingPressureScore(bKings, wKnightAttacks, wBishopAttacks, wRookAttacks, wQueenAttacks,  Color::BLACK, kingSafetyTable);
        features += "Final Score: " + std::to_string(score) + "\n";
        std::cout << pieces << std::endl;
        return features;
}



int main() {
    std::string fen = "rnbqkbnr/pp3ppp/2p1p3/3p4/3P4/4PN2/PPP2PPP/RNBQKB1R w KQkq - 0 4";
    Board board = Board(fen);


    // Retrieve the features string. Make sure this string is sanitized or does not contain double quotes.
    std::string features = testEvaluation(board, false, baseEval); // Ensure this does not generate syntax-breaking characters

    // Construct the command using double quotes for arguments
    std::string command = "python dbs/evaluation_tester.py \"" + fen + "\" \"" + features + "\"";
    system(command.c_str());

    return 0;
}