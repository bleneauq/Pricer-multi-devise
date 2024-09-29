#include "Asset.hpp"

Asset::Asset(std::string id, PnlVect* volatilityVector, double spot, double rf){
    this->id_ = id;
    this->domesticInterestRate_ = rf;
    this->volatilityVector_ = volatilityVector;
    this->spot_ = spot;
    this->volatilityChol_ = pnl_vect_create(volatilityVector_->size);
    // Drift is equal to
}

void Asset::SetDriftAndVol(PnlVect *FXVolatilityVector, PnlMat *cholesky){
    //PnlVect *vol = volatilityVector_; // We get the asset intrinseque volatilityVector pointer
    if(id_ != "eur"){
        pnl_vect_plus_vect(volatilityVector_, FXVolatilityVector);
    }
    this->volatilityChol_ = pnl_mat_mult_vect(cholesky, this->volatilityVector_);
    drift_ = domesticInterestRate_ - pow(pnl_vect_norm_two(volatilityChol_), 2)/2;
}
