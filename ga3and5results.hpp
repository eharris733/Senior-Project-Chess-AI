#include "baselines.hpp"
TunableSearch Games10Ga5 = {
    12, // Aspiration Window initial delta
    4, // Use Aspiration Window Depth
    1, // Use Lazy Eval Static
    180, // Futility Margin 
    159, // Razoring Margin 
    974, // Delta Margin
    794, // Killer Move Score
    2, // Initial LMR Depth
    3, // Initial Move Counter
    14, // LMP Move Count
    2, // Null Move Pruning Initial Reduction
    20, // Null Move Pruning Depth Factor
};