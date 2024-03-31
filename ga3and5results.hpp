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