#include "ForeignAsian.hpp"
#include <cmath>

// Constructor inherited from Option.hpp
ForeignAsian::ForeignAsian(int T, std::vector<int> fixingDates, std::vector<int> assetCurrencyMapping, double domesticInterestRate, int nbOfDaysInOneYear){
    this->assetCurrencyMapping_ = assetCurrencyMapping;
    this->T_ = T;
    this->fixingDates_ = fixingDates;
    this->domesticInterestRate_ = domesticInterestRate;
    this->nbOfDaysInOneYear_ = nbOfDaysInOneYear;
}

double ForeignAsian::payoff(const PnlMat* path) {
    double sum = 0;
    int nbDates = fixingDates_.size();
    for (int ti = 0; ti < nbDates; ti++) {
        sum += pnl_mat_get(path, ti + 1, 1); // colulmn 0 is Domestic Risked, 
    }
    return std::fmax(0.0, (1/(double)nbDates)*sum - pnl_mat_get(path, path->m - 1, 0));
}

ForeignAsian::~ForeignAsian(){
}