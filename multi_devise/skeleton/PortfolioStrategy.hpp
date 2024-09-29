#pragma once
#include "pnl/pnl_vector.h"
#include "Portfolio.hpp"
#include "DataReader.hpp"

class PortfolioStrategy {
public:
    Portfolio *hedgingPortfolio_;
    PnlMat *path_;
    PnlRng *rng_;
    PnlMat *marketData_;
    PnlVect* lastPrices_;
    int lastRebalancingDate_;
    DataReader *data_;
    MonteCarlo *monteCarlo_;
    int currentLine_;
    std::vector<int> daysGrid_;

    PortfolioStrategy(DataReader *data, PnlMat *marketData, MonteCarlo *mc, PnlRng *rng);
    void updateSpot(int dayNb);
    void updateSpotAssetCurrencies(PnlVect* newSpots, int dayNb);
    void rebalancePortfolio(int fixingAlreadySeen, int timeToNextFixing, int dayNb);
    double getPortfolioValue(PnlVect *currentAssetValues, PnlVect* newAssetComposition, int currentDate);
    ~PortfolioStrategy();
};
