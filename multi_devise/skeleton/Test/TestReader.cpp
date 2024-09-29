#include <iostream>
#include <ctime>
#include <fstream>
#include "../json_reader.hpp"
#include "../DataReader.hpp"
#include "pnl/pnl_matrix.h"


int main(int argc, char **argv) {
    std::ifstream ifs(argv[1]);
    nlohmann::json jsonParams = nlohmann::json::parse(ifs);

    // Read the content from the Json to a C++ object
    DataReader data = DataReader(jsonParams);
    pnl_mat_print(data.correlation_);

    for (int i = 0; i < data.assetCurrencyMapping_.size(); i++) {
        std::cout << data.assetCurrencyMapping_[i] << std::endl;
    }
}