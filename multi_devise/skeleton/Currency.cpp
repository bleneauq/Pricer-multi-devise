#include "Currency.hpp"

Currency::Currency(std::string id, PnlVect* volatilityVector, double interestRate, double spot) {
    this->id_ = id;
    this->volatilityVector_ = volatilityVector;
    this-> interestRate_ = interestRate;
    this->volatilityChol_ = pnl_vect_create(volatilityVector_->size);
    this->spot_ = spot;
}

void Currency::SetDriftAndVol(double domesticInterestRate, PnlMat *cholesky){
    this->volatilityChol_ = pnl_mat_mult_vect(cholesky, this->volatilityVector_);
    //drift_ = domesticInterestRate - interestRate_ - pow(pnl_vect_norm_two(volatilityChol_), 2)/2;
    drift_ = domesticInterestRate - pow(pnl_vect_norm_two(volatilityChol_), 2)/2;
}
