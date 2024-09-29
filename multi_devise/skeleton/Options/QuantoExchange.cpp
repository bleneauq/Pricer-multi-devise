#include "QuantoExchange.hpp"
#include <cmath>

// Constructor inherited from Option.hpp
QuantoExchange::QuantoExchange(int T, double strike, std::vector<int> assetCurrencyMapping, double domesticInterestRate, int nbOfDaysInOneYear){
    this->assetCurrencyMapping_ = assetCurrencyMapping;
    this->T_ = T;
    this->strike_ = strike;
    this->domesticInterestRate_ = domesticInterestRate;
    this->nbOfDaysInOneYear_ = nbOfDaysInOneYear;
}

double QuantoExchange::payoff(const PnlMat* path) {
    double finalDomAsset;
    double finalForeignAsset;
    if (assetCurrencyMapping_[0] == 0 && assetCurrencyMapping_[1] == 1) {
        finalDomAsset = pnl_mat_get(path, path->m - 1, 0);
        finalForeignAsset = pnl_mat_get(path, path->m - 1, 1);
    } else {
        finalDomAsset = pnl_mat_get(path, path->m - 1, 1);
        finalForeignAsset = pnl_mat_get(path, path->m - 1, 0);
    }
    return std::fmax(0.0, finalDomAsset - finalForeignAsset - strike_);
}

QuantoExchange::~QuantoExchange(){
}