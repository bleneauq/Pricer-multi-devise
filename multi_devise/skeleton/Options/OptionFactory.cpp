#include "OptionFactory.hpp"
#include "CallCurrency.hpp"
#include "CallQuanto.hpp"
#include "ForeignAsian.hpp"
#include "ForeignPerfBasket.hpp"
#include "QuantoExchange.hpp"


Option* OptionFactory::buildOption(DataReader *data) {
    
    if (data->optionType_ == "call_currency") {
        return new CallCurrency(data->maturity_, data->strike_, data->assetCurrencyMapping_, data->foreignInterestRates_, data->domesticInterestRate_, data->numberOfDaysInOneYear_);
    } else if (data->optionType_ == "call_quanto") {
        return new CallQuanto(data->maturity_, data->strike_, data->assetCurrencyMapping_, data->domesticInterestRate_, data->numberOfDaysInOneYear_);
    } else if (data->optionType_ == "foreign_asian") {
        return new ForeignAsian(data->maturity_, data->fixingDates_, data->assetCurrencyMapping_, data->domesticInterestRate_, data->numberOfDaysInOneYear_);
    } else if (data->optionType_ == "foreign_perf_basket"){
        return new ForeignPerfBasket(data->maturity_, data->strike_, data->assetCurrencyMapping_, data->fixingDates_, data->foreignInterestRates_, data->nbAssets_);
    } else if (data->optionType_ == "quanto_exchange") {
        return new QuantoExchange(data->maturity_, data->strike_, data->assetCurrencyMapping_, data->domesticInterestRate_, data->numberOfDaysInOneYear_);
    } else {
            throw "Wrong option type!";
    }

}