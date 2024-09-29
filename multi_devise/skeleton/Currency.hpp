#pragma once
#include "RiskyAsset.hpp"
#include <string>
#include <math.h>
#include "pnl/pnl_vector.h"

class Currency : public RiskyAsset {
    public:
        double interestRate_;
        double spot_;
        std::string id_; 
        PnlVect *volatilityChol_;

        Currency(std::string id, PnlVect* volatilityVector, double interestRate, double spot);
        void SetDriftAndVol(double domesticInterestRate, PnlMat *correlation);
};