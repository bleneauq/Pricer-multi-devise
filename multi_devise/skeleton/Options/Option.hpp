#pragma once
#include "pnl/pnl_vector.h"
#include <vector>
#include <alloca.h>

/// \brief Classe Option abstraite
class Option
{
public:
    std::vector<int> assetCurrencyMapping_;
    std::vector<double> foreignInterestRates_;
    double domesticInterestRate_;
    double T_; // maturité

    /**
     * Calcule la valeur du payoff sur la trajectoire
     *
     * @param[in] path est une matrice de taille (dates_+1) x size_
     * contenant une trajectoire du modèle telle que créée
     * par la fonction asset.
     * @return phi(trajectoire)
     */
    virtual double payoff(const PnlMat *path) = 0;
};

