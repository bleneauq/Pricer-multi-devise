#include "GlobalModel.hpp"
#include <math.h>

using namespace std;

GlobalModel::GlobalModel(DataReader *data)
{
    assets_ = data->assets_;
    currencies_ = data->currencies_;
    L_ = data->correlation_;
    pnl_mat_chol(L_);
    assetCurrencyMapping_ = data->assetCurrencyMapping_;
    dateGrid_ = data->fixingDates_;
    nbAssetsDom_ = data->nbAssetsDom_;
    nbSample_ = data->sampleNb_;
    numberOfDaysInOneYear_ = data->numberOfDaysInOneYear_;

    totalNbOfAssets_ = assets_.size();
    totalSize_ = L_->n;
    matNormalDistrubition_ = pnl_mat_create(totalSize_, dateGrid_.size());

    spot_ = pnl_vect_create(totalSize_);
}

void GlobalModel::sample(PnlMat *path, PnlRng *rng, int t, int timeToNextFixing)
{
    pnl_mat_rng_normal(matNormalDistrubition_, totalSize_, dateGrid_.size(), rng);

    double spotTempo;

    double drift;
    double deltaTime;
    double facteur = 1;
    double simulatedSpotPrice = 1;
    PnlVect *brownianVector = pnl_vect_create(totalSize_);
    double lastDate = 0;

    //Diffusion of the Assets (domestic and foreign)
    for (int iAsset = 0; iAsset < totalNbOfAssets_; iAsset++)
    {
        simulatedSpotPrice = assets_[iAsset].spot_;
        deltaTime = timeToNextFixing/(double)numberOfDaysInOneYear_;
        for (int tIndex = t; tIndex < dateGrid_.size(); tIndex++)
        {
            pnl_mat_get_col(brownianVector, matNormalDistrubition_, tIndex);
            facteur = exp(assets_[iAsset].drift_ * deltaTime + sqrt(deltaTime) * pnl_vect_scalar_prod(assets_[iAsset].volatilityChol_, brownianVector));
            simulatedSpotPrice *= facteur;
            pnl_mat_set(path, tIndex + 1, iAsset, simulatedSpotPrice);
            lastDate = dateGrid_[tIndex];
            if (tIndex != dateGrid_.size() - 1)
            {
                deltaTime = (double)(dateGrid_[tIndex + 1] - lastDate) / (double)numberOfDaysInOneYear_;
            }
        }
    }
    //Diffusion of the FX rates
    lastDate = 0;
    for (int iFX = 1; iFX < currencies_.size(); iFX++)
    {
        simulatedSpotPrice = currencies_[iFX].spot_;
        deltaTime = timeToNextFixing/(double)numberOfDaysInOneYear_;
        for (int tIndex = t; tIndex < dateGrid_.size(); tIndex++)
        {
            pnl_mat_get_col(brownianVector, matNormalDistrubition_, tIndex);
            facteur = exp(currencies_[iFX].drift_ * deltaTime + sqrt(deltaTime) * pnl_vect_scalar_prod(currencies_[iFX].volatilityChol_, brownianVector));
            simulatedSpotPrice *= facteur;
            pnl_mat_set(path, tIndex + 1, assets_.size() + (iFX-1), simulatedSpotPrice);
            lastDate = dateGrid_[tIndex];
            if (tIndex != dateGrid_.size() - 1)
            {
                deltaTime = (double)(dateGrid_[tIndex + 1] - lastDate) / (double)numberOfDaysInOneYear_;
            }
        }
    }
}

void GlobalModel::shift(PnlMat *shiftedPath, double shifter, int firstShift, int assetToShift)
{
    double temp;
    for (int timeIndex = firstShift; timeIndex < shiftedPath->m; timeIndex++)
    {
        temp = MGET(shiftedPath, timeIndex, assetToShift);
        temp *= shifter;
        PNL_MSET(shiftedPath, timeIndex, assetToShift, temp);
    }
}

GlobalModel::~GlobalModel() {
    pnl_mat_free(&matNormalDistrubition_);
    pnl_mat_free(&L_);
    pnl_vect_free(&spot_);
}