#pragma once
#include <list>
#include "MonteCarlo.hpp"
#include "DataReader.hpp"

class Position {
public:
    int date;
    PnlVect *deltas;
    PnlVect *deltasStdDev;
    double price;
    double priceStdDev;
    double portfolioValue;

    Position(int date, double price, double priceStdDev, PnlVect* deltas, PnlVect* deltasStdDev, double portfolioValue);
    friend void to_json(nlohmann::json &j, const Position &positions);
    void print() const;
    ~Position();
};

class Portfolio {
public:
    std::list<Position> positions;
    double shareValues_;
    double cash_;
    PnlVect *shareQuantities_;

    Portfolio(PnlMat *correlationMatrix, double firstPrice);
    ~Portfolio();
};