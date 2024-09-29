// riskyAsset will have two implementations : asset AND currency

// common attributes are 

// double drift;
// PnlVect *volatilityVector
#pragma once
#include "pnl/pnl_vector.h"
#include "pnl/pnl_random.h"

class RiskyAsset {
    public:
        double drift_;
        PnlVect *volatilityVector_;
        //void diffuse(double drift, PnlVect *sigma, double T, PnlRng* rng);
    
};