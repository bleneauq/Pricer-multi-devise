#include "DataReader.hpp"
#include "json_reader.hpp"
#include "Currency.hpp"
#include "Asset.hpp"
#include <map>
#include <unordered_map>

// Implement Constructor correctly
DataReader::DataReader(nlohmann::json jsonParams)
{
    std::map<std::string, int> mapCurrencies;
    std::map<std::string, double> mapCurrenciesSpot;

    auto jsonCurrencies = jsonParams.at("Currencies");
    auto jsonAssets = jsonParams.at("Assets");
    this->nbAssets_ = jsonAssets.size();
    this->nbCurrencies_ = jsonCurrencies.size();

    // Currencies vector
    std::vector<Currency> currencies;;
    int index = 0;
    double domesticInterestRate;
    std::vector<double> foreignInterestRates;

    // Correlation matrix
    PnlMat *correlation;
    jsonParams.at("Correlations").get_to(correlation);
    this->correlation_ = pnl_mat_copy(correlation);
    this->cholesky_ = pnl_mat_copy(correlation);
    pnl_mat_chol(cholesky_);
    pnl_mat_free(&correlation);

    for (auto jsonCurrency : jsonCurrencies) {        
        std::string currencyId(jsonCurrency.at("id").get<std::string>());
        double rf = jsonCurrency.at("InterestRate").get<double>();
        double realVolatility = jsonCurrency.at("Volatility").get<double>();
        double spot;
        double drift;
        PnlVect* volatilityTempo = pnl_vect_create_from_zero(this->nbAssets_ + this->nbCurrencies_ - 1);
        if (currencyId == "eur") {
            domesticInterestRate = rf;
            spot = 1;
            drift = 0;
            mapCurrencies[currencyId] = 0;
        } else {
            spot = jsonCurrency.at("Spot").get<double>();
            drift = jsonCurrency.at("Drift").get<double>();
            mapCurrencies[currencyId] = index;
            pnl_vect_set(volatilityTempo, nbAssets_ + index - 1, realVolatility);
            foreignInterestRates.push_back(rf);
        }
        mapCurrenciesSpot[currencyId] = spot;
        currencies.emplace_back(currencyId, volatilityTempo, rf, spot);
        currencies[index].SetDriftAndVol(domesticInterestRate, this->cholesky_);
        index++;
    }
    this->currencies_ = currencies;
    this->domesticInterestRate_ = domesticInterestRate;
    this->foreignInterestRates_ = foreignInterestRates;

    this->domesticCurrencyId_ = jsonParams.at("DomesticCurrencyId").get<std::string>();

    // Asset vector
    std::vector<int> assetCurrencyMapping;
    std::vector<Asset> assets;
    this->nbAssetsDom_ = 0;
    index = 0;
    for (auto jsonAsset : jsonAssets) {
        std::string currencyId(jsonAsset.at("CurrencyId").get<std::string>());
        double realVolatility = jsonAsset.at("Volatility").get<double>();
        PnlVect* volatilityTempo = pnl_vect_create_from_zero(this->nbAssets_ + this->nbCurrencies_ - 1);
        pnl_vect_set(volatilityTempo, index, realVolatility);
        double spot = jsonAsset.at("Spot").get<double>();
        spot *= mapCurrenciesSpot[currencyId];
        double drift = jsonAsset.at("Drift").get<double>();
        assets.emplace_back(currencyId, volatilityTempo, spot, domesticInterestRate);
        if (currencyId == "eur") {
            this->nbAssetsDom_++;
        }
        assetCurrencyMapping.push_back(mapCurrencies[currencyId]);
        assets[index].SetDriftAndVol(currencies[assetCurrencyMapping[index]].volatilityVector_, this->cholesky_);
        index++;
    }
    this->assets_ = assets;
    this->assetCurrencyMapping_ = assetCurrencyMapping;
    this->numberOfDaysInOneYear_ = jsonParams.at("NumberOfDaysInOneYear");

    // Option
    this->optionType_ = jsonParams.at("Option").at("Type").get<std::string>();
    
    if (jsonParams.at("Option").contains("Strike")) {
        // La clé "Strike" existe dans le fichier JSON
        this->strike_ = jsonParams.at("Option").at("Strike").get<double>();
    }

    // Sample Number
    this->sampleNb_ = jsonParams.at("SampleNb").get<double>();

    this->finiteDifferenceStep_ = jsonParams.at("RelativeFiniteDifferenceStep").get<double>();

    this->maturity_ = jsonParams.at("Option").at("MaturityInDays").get<int>();
    std::vector<int> relabalancingDates = setGrid(jsonParams.at("PortfolioRebalancingOracleDescription"));
    this->rebalancingPeriod_ = jsonParams.at("PortfolioRebalancingOracleDescription").at("Period").get<int>();

    std::vector<int> fixingDates = setGrid(jsonParams.at("Option").at("FixingDatesInDays"));

    this->relabalancingDates_ = relabalancingDates;
    this->fixingDates_ = fixingDates;
}

std::vector<int> DataReader::setGrid(nlohmann::json j) {
    if (j.at("Type") == "Grid") {
        std::vector<int> grid =  j.at("DatesInDays").get<std::vector<int>>();
        return grid;
    } else {
        int period = j.at("Period").get<int>();
        int nbStep = (int) maturity_/period;

        std::vector<int> grid;
        grid.reserve(nbStep);
        
        for (int i = 1; i < nbStep+1; i++) {
            grid.push_back(period * i); 
        }
        //We can now update the maturity to a numberOfDays to numberOfYearsr
        maturity_ = maturity_;
        return grid;
    }
}

DataReader::~DataReader(){

}
