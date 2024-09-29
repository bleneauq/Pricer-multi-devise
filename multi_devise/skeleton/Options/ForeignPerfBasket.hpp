#pragma once

#include "Option.hpp"
#include <iostream>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

class ForeignPerfBasket : public Option {
public:
    double strike_;
    PnlVect *Pi_;
    PnlVect *denom_;
    int nbAssets_;
    
    // Constructor for ForeignPerfBasket
    ForeignPerfBasket(int T, double strike, std::vector<int> assetCurrencyMapping, std::vector<int> dateGrid, std::vector<double> foreignInterestRates, int nbAssets_);

    // Calculate the ForeignPerfBasket payoff
    double payoff(const PnlMat *path);

    ~ForeignPerfBasket();
};