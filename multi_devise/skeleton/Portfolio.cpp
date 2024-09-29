#include <iostream>
#include "Portfolio.hpp"
#include "json_reader.hpp"
#include <unordered_map>


Position::Position(int date, double price, double priceStdDev, PnlVect* deltas, PnlVect* deltasStdDev, double portfolioValue)
    : date(date), price(price), priceStdDev(priceStdDev), portfolioValue(portfolioValue), deltas(deltas), deltasStdDev(deltasStdDev) {
}

void to_json(nlohmann::json &j, const Position &position) {
    j["date"] = position.date;
    j["value"] = position.portfolioValue;
    j["price"] = position.price;
    j["priceStdDev"] = position.priceStdDev;
    j["deltas"] = position.deltas;
    j["deltasStdDev"] = position.deltasStdDev;
}

Position::~Position() {
    pnl_vect_free(&deltas);
    pnl_vect_free(&deltasStdDev);
}

void Position::print() const {
    nlohmann::json j = *this;
    std::cout << j.dump(4);
}

Portfolio::Portfolio(PnlMat* correlationMatrix, double firstPrice) {
    cash_ = firstPrice;
    shareValues_ = 0;
    shareQuantities_ = pnl_vect_create_from_zero(correlationMatrix->n);
}

Portfolio::~Portfolio() {
}
