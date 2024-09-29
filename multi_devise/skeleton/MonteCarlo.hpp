#pragma once
#include <iostream>
#include <nlohmann/json.hpp>
#include "Options/Option.hpp"
#include "GlobalModel.hpp"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

class MonteCarlo {
    public: 
        Option* option_;
        GlobalModel* model_;
        double fdStep_;

        MonteCarlo(GlobalModel* model, Option* option, double fdStep);
        void processDeltasPostSimu(PnlMat *path, PnlVect *deltas, PnlVect *deltasStdDev, int t);
        void deltasCompute(PnlMat *path, PnlVect *deltas, PnlVect *deltasStdDev, PnlMat *shiftedPath, double &payoffShiftedPlus, double &payoffShiftedMinus, int t);
        void priceAndDelta(int t, PnlMat *path, double &price, double &priceStdDev, PnlVect *deltas, PnlVect *deltasStdDev, PnlRng *rng, int timeToNextFixing);
};