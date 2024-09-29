#include "Asset.hpp"
#include "Currency.hpp"
#include <iostream>
#include <string>
#include <fstream>

#include "pnl/pnl_matrix.h"
#include "pnl/pnl_vector.h"
#include "DataReader.hpp"

using namespace std;


class GlobalModel{
    // Will contain a c++ array of Asset objects

    // We need to retrieve ther number of assets
    public :
        vector<Asset> assets_;
        vector<Currency> currencies_;

        int totalNbOfAssets_;
        int totalSize_;
        int nbAssetsDom_;
        int nbSample_;
        int numberOfDaysInOneYear_;

        vector<int> dateGrid_;

        vector<int> assetCurrencyMapping_;

        //PnlMat *correlationMatrix_; // size nbOfAssets x nbOfAssets
        PnlVect *spot_; // size totalSize_

        PnlMat *L_; // L_ will be the Cholesky of correlationMatrix_
        PnlMat *matNormalDistrubition_;
        //PnlMat *path_;

        GlobalModel(DataReader *data);// constructor, will need to create the two arrays of currencies and 
        //void discount();
        //void sample(PnlMat* path, PnlRng* rng);
        void sample(PnlMat* path, PnlRng* rng, int t, int timeToNextFixing);
        void shift(PnlMat *path, double shifter, int firstShift, int assetToShift);
        ~GlobalModel();
};