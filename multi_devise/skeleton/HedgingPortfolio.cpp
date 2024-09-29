#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "json_reader.hpp"
#include "DataReader.hpp"
#include "PortfolioStrategy.hpp"
#include "MonteCarlo.hpp"
#include "Options/OptionFactory.hpp"

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Wrong number of arguments. Exactly 3 arguments are required" << std::endl;
        std::exit(0);
    }
    std::ifstream ifsParams(argv[1]);
    nlohmann::json jsonParams = nlohmann::json::parse(ifsParams);

    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));

    DataReader *data = new DataReader(jsonParams);

    Option *option = OptionFactory::buildOption(data);
    GlobalModel *model = new GlobalModel(data);
    MonteCarlo *mc = new MonteCarlo(model, option, data->finiteDifferenceStep_);
    PnlMat *marketData = pnl_mat_create_from_file(argv[2]);

    PortfolioStrategy portfolioStrategy = PortfolioStrategy(data, marketData, mc, rng);

    int fixingAlreadySeen = 0;
    int timeToNextFixing = 0;

    if (data->fixingDates_[fixingAlreadySeen] == 0)
    {
        fixingAlreadySeen += 1;
    }

    for (int date = 1; date <= data->fixingDates_.back(); date++)
    {
        std::cout << "date : " << date << " sur " << data->fixingDates_.back() << std::endl;
        if (data->fixingDates_[fixingAlreadySeen] == date)
        {
            fixingAlreadySeen += 1;
        }
        timeToNextFixing = data->fixingDates_[fixingAlreadySeen] - date;
        portfolioStrategy.updateSpot(date);
        if ((date % data->rebalancingPeriod_) == 0)
        {
            portfolioStrategy.rebalancePortfolio(fixingAlreadySeen, timeToNextFixing, date);
        }
    }

    // Print the positions into a json file
    nlohmann::json jsonPositions = portfolioStrategy.hedgingPortfolio_->positions;
    std::ofstream ifout(argv[3], std::ios_base::out);
    if (!ifout.is_open())
    {
        std::cout << "Unable to open file " << argv[3] << std::endl;
        std::exit(1);
    }
    ifout << jsonPositions.dump(4);
    ifout.close();

    // Free every objects and data structures 
    delete data;
    delete option;
    delete model;
    delete mc;

    pnl_rng_free(&rng);
    pnl_mat_free(&marketData);
    std::exit(0);
}
