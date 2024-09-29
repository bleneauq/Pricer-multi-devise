#pragma once
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Option.hpp"

class ForeignAsian: public Option
{
    
    // Constructor inherited from Option.hpp
    public:
        std::vector<int> fixingDates_;
        int nbOfDaysInOneYear_;
        
        ForeignAsian(int T, std::vector<int> fixingDates, std::vector<int> assetCurrencyMapping, double domesticInterestRate, int nbOfDaysInOneYear);
        
        double payoff(const PnlMat* path);
        ~ForeignAsian();
};
