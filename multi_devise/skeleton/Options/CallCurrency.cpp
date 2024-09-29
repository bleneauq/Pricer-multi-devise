#include "CallCurrency.hpp"
#include <cmath>

// Constructor inherited from Option.hpp
CallCurrency::CallCurrency(double T, double strike, std::vector<int> assetCurrencyMapping, std::vector<double> foreignInterestRates, double domesticInterestRate, int nbOfDaysInOneYear){
    this->assetCurrencyMapping_ = assetCurrencyMapping;
    this->foreignInterestRates_ = foreignInterestRates;
    this->T_ = T;
    this->strike_ = strike;
    this->domesticInterestRate_ = domesticInterestRate;
    this->nbOfDaysInOneYear_ = nbOfDaysInOneYear;
}

double CallCurrency::payoff(const PnlMat* path) {
    double foreignChangeRate = pnl_mat_get(path, 1, 0);
    double matuInYears = T_/(double)this->nbOfDaysInOneYear_;
    return std::fmax(0.0, foreignChangeRate - strike_);
}

CallCurrency::~CallCurrency(){
}