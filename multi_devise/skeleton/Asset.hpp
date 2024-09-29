#pragma once
#include "json_reader.hpp"
#include "RiskyAsset.hpp"
#include <iostream>
#include <string>
#include <fstream>

class Asset : public RiskyAsset{
    // on ajoute en plus le domesticInterestRate lié à l'asset (dom
    // ou foreign )
    public:
        double domesticInterestRate_;
        double spot_;
        std::string id_;
        PnlVect *volatilityChol_;

        Asset(std::string id, PnlVect* volatilityVector, double spot, double rf);
        void SetDriftAndVol(PnlVect *FXVolatilityVector, PnlMat *correlation);
};