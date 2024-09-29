#pragma once
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Option.hpp"

class QuantoExchange: public Option
{
    
    // Constructor inherited from Option.hpp
    public:
        double strike_;
        int nbOfDaysInOneYear_;

        QuantoExchange(int T, double strike, std::vector<int> assetCurrencyMapping, double domesticInterestRate, int nbOfDaysInOneYear);
        
        double payoff(const PnlMat* path);
        ~QuantoExchange();
};
