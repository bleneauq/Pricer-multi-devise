#include "PortfolioStrategy.hpp"

PortfolioStrategy::PortfolioStrategy(DataReader *data, PnlMat *marketData, MonteCarlo *monteCarlo, PnlRng *rng) : monteCarlo_(monteCarlo), rng_(rng), data_(data)
{
    path_ = pnl_mat_create_from_zero(monteCarlo->model_->dateGrid_.size() + 1, monteCarlo->model_->totalSize_);
    lastPrices_ = pnl_vect_create_from_zero(data_->correlation_->n);
    currentLine_ = 0;
    daysGrid_ = data_->fixingDates_;
    lastRebalancingDate_ = 0;
    marketData_ = marketData;

    double price;
    double priceStdDev;
    PnlVect *deltas = pnl_vect_create_from_zero(data_->correlation_->n);
    PnlVect *deltasStdDev = pnl_vect_create_from_zero(data_->correlation_->n);
    
    std::cout << "date : " << 0 << " sur " << data->fixingDates_.back() << std::endl;
    monteCarlo_->priceAndDelta(0, path_, price, priceStdDev, deltas, deltasStdDev, rng_, daysGrid_[0]);

    updateSpot(0);
    hedgingPortfolio_ = new Portfolio(data_->correlation_, price);

    // first rebalancing in 0
    double portfolioValue = getPortfolioValue(lastPrices_, deltas, 0);
    
    // We add the first position => portfolio value should be equal to the option price
    hedgingPortfolio_->positions.emplace_back(0, price, priceStdDev, deltas, deltasStdDev, portfolioValue);

}

void PortfolioStrategy::updateSpot(int dayNb)
{   
    // => X_t.exp(R_f.t)
    pnl_mat_get_row(lastPrices_, marketData_, dayNb);
    int nbAsset = data_->assets_.size();
    for (int i = nbAsset; i < path_->n; i++) {
        pnl_vect_set(lastPrices_, i, pnl_vect_get(lastPrices_, i) * exp(data_->foreignInterestRates_[i - nbAsset] * (double)dayNb / (double)data_->numberOfDaysInOneYear_));
    }

    // We convert the foreign asset in market data into domestic ones => X_t.S_t
    int currId;
    for (int i = 0; i < nbAsset; i++)
    {
        currId = data_->assetCurrencyMapping_[i];
        if (currId > 0)
        {
            pnl_vect_set(lastPrices_, i, pnl_vect_get(lastPrices_, i) * pnl_vect_get(lastPrices_, currId + data_->assets_.size() - 1));
        }
    }
    
    this->updateSpotAssetCurrencies(lastPrices_, dayNb);

    // Add to path if in fixing dates
    if (dayNb == 0)
    {
        pnl_mat_set_row(path_, lastPrices_, 0);
    }
    if (data_->fixingDates_[currentLine_] == dayNb)
    {
        currentLine_++;
        pnl_mat_set_row(path_, lastPrices_, currentLine_);
    }
}

// Function to update to spots (with domestic values) into our assets and currencies vector
void PortfolioStrategy::updateSpotAssetCurrencies(PnlVect *newSpot, int dayNb)
{
    int i = 0;
    int nbAssets = monteCarlo_->model_->assets_.size();
    while (i < nbAssets) {
        // update the asset vector spots
        monteCarlo_->model_->assets_[i].spot_ = pnl_vect_get(newSpot, i);
        i++;
    }
    i = 0;
    while (i < monteCarlo_->model_->currencies_.size() - 1) {
        // update the currencies vector spots
        monteCarlo_->model_->currencies_[i + 1].spot_ = pnl_vect_get(newSpot, i + nbAssets);
        i++;
    }
}

void PortfolioStrategy::rebalancePortfolio(int fixingAlreadySeen, int timeToNextFixing, int dayNb)
{
    double price;
    double priceStdDev;
    PnlVect *deltas = pnl_vect_create_from_zero(path_->n);
    PnlVect *deltasStdDev = pnl_vect_create_from_zero(path_->n);

    // We need to not give dayNb to priceAndDeltas
    monteCarlo_->priceAndDelta(fixingAlreadySeen, path_, price, priceStdDev, deltas, deltasStdDev, rng_, timeToNextFixing);

    // set the new position and rebalance
    double portfolioValue = getPortfolioValue(lastPrices_, deltas, dayNb);

    // Add the now position in the position vector of our portfolio
    hedgingPortfolio_->positions.emplace_back(dayNb, price, priceStdDev, deltas, deltasStdDev, portfolioValue);

}

// Rebalance the portfolio
double PortfolioStrategy::getPortfolioValue(PnlVect *assetValues, PnlVect *deltas, int currentDate) {
    hedgingPortfolio_->shareValues_ = pnl_vect_scalar_prod(hedgingPortfolio_->shareQuantities_, assetValues); // Get the value of the risky asset invested before rebalancing 
    hedgingPortfolio_->shareQuantities_ = deltas; // update the amount of shares needed
    double newStockValue = pnl_vect_scalar_prod(hedgingPortfolio_->shareQuantities_, assetValues); // Get the new value of the risky asset invested after rebalancing
    double newCash = hedgingPortfolio_->shareValues_ - newStockValue; // new cash to invest along the difference of risky assets invested
    hedgingPortfolio_->shareValues_ = newStockValue; 
    hedgingPortfolio_->cash_ = hedgingPortfolio_->cash_ * exp(data_->domesticInterestRate_ * (double)( currentDate - lastRebalancingDate_) / (double)data_->numberOfDaysInOneYear_) + newCash; // compute the new amount of cash needed (cash before rebalancing discounted + new cash )
    lastRebalancingDate_ = currentDate;

    return (pnl_vect_scalar_prod(hedgingPortfolio_->shareQuantities_, assetValues) + hedgingPortfolio_->cash_ ); // Compute the final portfolio value (risky assets + cash)
}


PortfolioStrategy::~PortfolioStrategy()
{
    pnl_mat_free(&marketData_);
    pnl_mat_free(&path_);
    pnl_vect_free(&lastPrices_);
    pnl_rng_free(&rng_);
}
