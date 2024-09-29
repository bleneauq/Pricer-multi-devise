#pragma once

#include "json_reader.hpp"
#include "Currency.hpp"
#include "Asset.hpp"
#include <iostream>
#include <string>
#include <fstream>
    
class DataReader
{
  public:
    std::string domesticCurrencyId_;
    int numberOfDaysInOneYear_;
    int nbAssets_;
    int nbAssetsDom_;
    int nbCurrencies_;
    std::vector<Currency> currencies_;
    std::vector<Asset> assets_;
    std::vector<int> assetCurrencyMapping_;
    PnlMat *correlation_;
    PnlMat *cholesky_;
    double sampleNb_;
    double finiteDifferenceStep_;
    std::string optionType_;
    int maturity_;
    double strike_;
    std::vector<int> relabalancingDates_;
    int rebalancingPeriod_;
    std::vector<int> fixingDates_;
    double domesticInterestRate_;
    std::vector<double> foreignInterestRates_;

    DataReader(nlohmann::json j); 
    ~DataReader();

  private: 
    std::vector<int> setGrid(nlohmann::json j);
};
