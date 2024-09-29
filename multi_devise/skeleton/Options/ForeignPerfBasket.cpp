#include "ForeignPerfBasket.hpp"
#include <iostream>
#include <cmath>


ForeignPerfBasket::ForeignPerfBasket(int maturityDays, double strike, std::vector<int> assetCurrencyMapping, std::vector<int> dateGrid, std::vector<double> foreignInterestRates, int nbAssets)
:strike_(strike){
    assetCurrencyMapping_ = assetCurrencyMapping;
    foreignInterestRates_ = foreignInterestRates;
    T_ = maturityDays;
    nbAssets_ = nbAssets;
    Pi_ = pnl_vect_create_from_zero(foreignInterestRates.size());
    denom_ = pnl_vect_create_from_zero(foreignInterestRates.size());
}

double ForeignPerfBasket::payoff(const PnlMat *path) {
    int nbFor = 0;
    int nbDom = 0;
    double sumFor = 0;
    double sumDom = 0;
    for (int i = 0; i < nbAssets_; i++){
        if (assetCurrencyMapping_[i] != 0){
            pnl_vect_set(Pi_, assetCurrencyMapping_[i] - 1, pnl_mat_get(path, 1 ,i) / pnl_mat_get(path,1, nbAssets_ - 1 + assetCurrencyMapping_[i]) + pnl_vect_get(Pi_, assetCurrencyMapping_[i] - 1));
            pnl_vect_set(denom_, assetCurrencyMapping_[i] - 1 , pnl_mat_get(path,0 ,i) / pnl_mat_get(path,0, nbAssets_ - 1 + assetCurrencyMapping_[i]) + pnl_vect_get(denom_, assetCurrencyMapping_[i] - 1));
        }
    }

    pnl_vect_div_vect_term(Pi_, denom_);
    
    double max = 0;
    int indexMax = 0;

    pnl_vect_max_index(&max, &indexMax, Pi_);

    for (int i = 0; i < nbAssets_; i++){
        if (assetCurrencyMapping_[i] == indexMax +1){
            sumFor += pnl_mat_get(path, path->m - 1, i);
            nbFor ++;
        }
        else if (assetCurrencyMapping_[i] == 0){
            sumDom = pnl_mat_get(path, path->m - 1, i);
            nbDom ++;
        }
    }

    pnl_vect_set_all(denom_, 0);
    pnl_vect_set_all(Pi_, 0);

    return std::fmax(0.0, (sumFor/nbFor - sumDom/nbDom - strike_));
}

ForeignPerfBasket::~ForeignPerfBasket(){
    pnl_vect_free(&Pi_);
    pnl_vect_free(&denom_);
}