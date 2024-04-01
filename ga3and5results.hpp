#include "baselines.hpp"
TunableSearch result100ga5 = {
    36, // Aspiration Window 1
    129, // Aspiration Window 2
    19, // Aspiration Window initial delta
    5, // Use Aspiration Window Depth
    1, // Use Lazy Eval NMR
    82, // Futility Margin 1
    686, // Futility Margin 2
    270, // Futility Margin 3
    0, // Use Lazy Eval Futility
    860, // Delta Margin
    773, // Promotion Move Score
    611, // Killer Move Score
    5, // Initial LMR Depth
    3, // Secondary Depth LMR
    7, // Initial Move Counter
    15, // Secondary Move Counter
};
// both this and the previous also have much better results than the baseline
TunableSearch result200ga5 = {
    36, // aspirationWindow1
    129, // aspirationWindow2
    19, // aspirationWindowInitialDelta
    5, // useAspirationWindowDepth

    1, // useLazyEvalNMR

    83, // futilityMargin1
    686, // futilityMargin2
    273, // futilityMargin3
    0, // useLazyEvalFutility

    860, // deltaMargin

    762, // promotionMoveScore
    611, // killerMoveScore

    5, // initalDepthLMR
    4, // secondaryDepthLMR
    7, // initialMoveCountLMR
    15, // secondaryMoveCountLMR
};


//results are much better than baseline
TunableSearch resultGAExpiremental = {
63, // Aspiration Window 1
71, // Aspiration Window 2
21, // Aspiration Window initial delta
0, // Use Aspiration Window Depth
0, // Use Lazy Eval NMR
854, // Futility Margin 1
683, // Futility Margin 2
753, // Futility Margin 3
1, // Use Lazy Eval Futility
385, // Delta Margin
283, // Promotion Move Score
715, // Killer Move Score
7, // Initial LMR Depth
9, // Secondary Depth LMR
6, // Initial Move Counter
4, // Secondary Move Counter
};

// this result is from the king of the hill method
TunableSearch resultKingHill1 = {
247, // Aspiration Window 1
122, // Aspiration Window 2
18, // Aspiration Window initial delta
4, // Use Aspiration Window Depth
0, // Use Lazy Eval NMR
711, // Futility Margin 1
585, // Futility Margin 2
124, // Futility Margin 3
1, // Use Lazy Eval Futility
223, // Delta Margin
591, // Promotion Move Score
488, // Killer Move Score
6, // Initial LMR Depth
15, // Secondary Depth LMR
7, // Initial Move Counter
8, // Secondary Move Counter
};





//   //tuneable search parameters

//     // for aspiration window
//     int aspirationWindow1 = 0;
//     int aspirationWindow2 = 0;
//     int aspirationWindowInitialDelta = 0;
//     int useAspirationWindowDepth = 0;

//     // for null move reductions
//     bool useLazyEvalNMR = 0;

//     // for futility pruning
//     int futilityMargin1 =0;
//     int futilityMargin2 = 0;
//     int futilityMargin3 = 0;
//     bool useLazyEvalFutility = true;
//     // for delta pruning in QS search
//     int deltaMargin = 0;

//     // for move ordering
//     int promotionMoveScore = 0;
//     int killerMoveScore = 0;

//     //for late move reductions
//     int initalDepthLMR = 0;
//     int secondaryDepthLMR = 0;
//     int initialMoveCountLMR = 0;
//     int secondaryMoveCountLMR = 0;