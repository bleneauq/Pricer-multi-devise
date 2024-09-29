#include <iostream>
#include <ctime>
#include <fstream>
#include "../json_reader.hpp"
#include "../DataReader.hpp"
#include "../MonteCarlo.hpp"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_random.h"
#include "../Options/OptionFactory.hpp"

int main(int argc, char **argv) {
    std::ifstream ifs(argv[1]);
    nlohmann::json jsonParams = nlohmann::json::parse(ifs);

    // Read the content from the Json to a C++ object
    DataReader *data = new DataReader(jsonParams);

    //PnlMat* market = pnl_vect_create_from_file("../data/call_currency_market.csv")

    GlobalModel *model = new GlobalModel(data);
    
    PnlMat *path = pnl_mat_create_from_zero(model->dateGrid_.size() + 1, model->totalSize_);

    PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));

    Option *option = OptionFactory::buildOption(data);
    // double payoff = option->payoff(path);
    // pnl_mat_print(path);

    MonteCarlo mc = MonteCarlo(model, option, data->finiteDifferenceStep_);
    double price;
    double priceStdDev;
    PnlVect *deltas = pnl_vect_create(path->n);
    PnlVect *deltaStdDev = pnl_vect_create(path->n);
    mc.priceAndDelta(0, path, price, priceStdDev, deltas, deltaStdDev, rng, model->dateGrid_[0]);

    // std::cout << "payoff :" << payoff << std::endl;
    std::cout << "price : " << price << std::endl;
    std::cout << "stdDev for price : " << priceStdDev << std::endl;

    std::cout << "deltas : " << std::endl;
    pnl_vect_print(deltas);
    std::cout << "deltasStdDev" << std::endl;
    pnl_vect_print(deltaStdDev);

}