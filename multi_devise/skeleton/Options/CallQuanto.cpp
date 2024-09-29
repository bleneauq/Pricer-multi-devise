#include "CallQuanto.hpp"
#include <cmath>

// Constructor inherited from Option.hpp
CallQuanto::CallQuanto(int T, double strike, std::vector<int> assetCurrencyMapping, double domesticInterestRate, int nbOfDaysInOneYear){
    this->assetCurrencyMapping_ = assetCurrencyMapping;
    this->T_ = T;
    this->strike_ = strike;
    this->domesticInterestRate_ = domesticInterestRate;
    this->nbOfDaysInOneYear_ = nbOfDaysInOneYear;
}

double CallQuanto::payoff(const PnlMat* path) {
    double matuInYears = T_/(double)this->nbOfDaysInOneYear_;
    double finalDomAsset = pnl_mat_get(path, path->m - 1, 0.0);
    return std::fmax(0.0, finalDomAsset - strike_);
}

CallQuanto::~CallQuanto(){
}