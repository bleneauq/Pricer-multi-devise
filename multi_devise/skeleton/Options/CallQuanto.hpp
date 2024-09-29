#pragma once
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Option.hpp"


class CallQuanto: public Option
{
    
    // Constructor inherited from Option.hpp
    public:
        double strike_;
        int nbOfDaysInOneYear_;
        

        CallQuanto(int T, double strike, std::vector<int> assetCurrencyMapping, double domesticInterestRate, int nbOfDaysInOneYear);
        
        double payoff(const PnlMat* path);
        ~CallQuanto();
};
