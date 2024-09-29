#pragma once
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Option.hpp"

class CallCurrency: public Option
{
    
    // Constructor inherited from Option.hpp
    public:
        double strike_;
        int nbOfDaysInOneYear_;

        CallCurrency(double T, double strike, std::vector<int> assetCurrencyMapping, std::vector<double> foreignInterestRates, double domesticInterestRate, int nbOfDaysInOneYear);
        
        double payoff(const PnlMat* path);
        ~CallCurrency();
};
