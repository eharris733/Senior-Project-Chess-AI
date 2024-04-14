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

TunableSearch Games100Ga5BaseOpp = {
    25, // Aspiration Window initial delta
2, // Use Aspiration Window Depth
1, // Use Lazy Eval Static
82, // Futility Margin 
185, // Razoring Margin 
213, // Delta Margin
772, // Killer Move Score
3, // Initial LMR Depth
7, // Initial Move Counter
8, // LMP Move Count
3, // Null Move Pruning Initial Reduction
25, // Null Move Pruning Depth Factor
};

TunableSearch resultX = {
46, // Aspiration Window initial delta
2, // Use Aspiration Window Depth
1, // Use Lazy Eval Static
124, // Futility Margin 
159, // Razoring Margin 
129, // Delta Margin
364, // Killer Move Score
3, // Initial LMR Depth
2, // Initial Move Counter
7, // LMP Move Count
2, // Null Move Pruning Initial Reduction
7, // Null Move Pruning Depth Factor
};