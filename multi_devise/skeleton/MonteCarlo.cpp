#include "MonteCarlo.hpp"
#include <math.h>

MonteCarlo::MonteCarlo(GlobalModel* model, Option *option, double fdStep) {
    model_ = model;
    option_ = option;
    fdStep_ = fdStep;
}

void MonteCarlo::priceAndDelta(int t, PnlMat *path, double &price, double &priceStdDev, PnlVect *deltas, PnlVect *deltasStdDev, PnlRng *rng, int timeToNextFixing){

    double totalPrice = 0;
    double squarePayoff = 0;
    double currentDate = t / (double)model_->numberOfDaysInOneYear_;
    double T = (double)option_->T_/(double)model_->numberOfDaysInOneYear_;
    double rd = option_->domesticInterestRate_;

    PnlMat *shiftedPath = pnl_mat_copy(path);

    double tempPayoff;
    double payoffShiftedPlus;
    double payoffShiftedMinus;

    pnl_vect_set_all(deltasStdDev, 0);
    pnl_vect_set_all(deltas, 0);

    for (long M = 0; M < model_->nbSample_; M++) {

        //std::cout << "iteration MC " << M << std::endl; 
        model_->sample(path, rng, t, timeToNextFixing);
        
        tempPayoff = option_->payoff(path);

        totalPrice += tempPayoff;
        squarePayoff += tempPayoff * tempPayoff;

        deltasCompute(path, deltas, deltasStdDev, shiftedPath, payoffShiftedPlus, payoffShiftedMinus, t);

    }
    processDeltasPostSimu(path, deltas, deltasStdDev, t);

    double payoffOption = totalPrice / model_->nbSample_;

    // Compute the price estimation and the stdDev associated to this estimation
    price = exp(-rd * (T - t)) * payoffOption;
    priceStdDev = sqrt(abs((exp(-2 * rd * (T - currentDate)) * squarePayoff / model_->nbSample_) - price * price) / model_->nbSample_);

    pnl_mat_free(&shiftedPath);
}


void MonteCarlo::deltasCompute(PnlMat *path, PnlVect *deltas, PnlVect *deltasStdDev, PnlMat *shiftedPath, double &payoffShiftedPlus, double &payoffShiftedMinus, int t){
    double tempSum;
    double tempSumSquare;
    double diff;
    double shifter;

    // Get the index of the first column to shift
    int firstShift = t;

    // Iterate the shifting over each asset
    for (int indexAsset = 0; indexAsset < path->n; indexAsset++) {
        pnl_mat_clone(shiftedPath, path);

        // Call the shift method, that will handle both shifting (*1+h and *1-h)
        pnl_mat_clone(shiftedPath, path);
        shifter = 1 + fdStep_;
        model_->shift(shiftedPath, shifter, firstShift, indexAsset);
        payoffShiftedPlus = option_->payoff(shiftedPath);

        pnl_mat_clone(shiftedPath, path);
        shifter = 1 - fdStep_;
        model_->shift(shiftedPath, shifter, firstShift, indexAsset);
        payoffShiftedMinus = option_->payoff(shiftedPath);

        diff = payoffShiftedPlus - payoffShiftedMinus;

        tempSum = GET(deltas, indexAsset);
        tempSumSquare = GET(deltasStdDev, indexAsset);

        // Store the raw values of the payoff differences, that will later be discounted by treatmentDeltasPostComputations (underneath)
        pnl_vect_set(deltas, indexAsset, tempSum + diff);
        pnl_vect_set(deltasStdDev, indexAsset, tempSumSquare + diff * diff);
    }
}

double Inverse(double x) {
    return 1 / x;
 }
double SquareInverse(double x) {
    return 1 / (x * x);
 }
double SquareRoot(double x) {
    return sqrt(abs(x));
 }

void MonteCarlo::processDeltasPostSimu(PnlMat *path, PnlVect *deltas, PnlVect *deltasStdDev, int t){
    PnlVect *copyDelta = pnl_vect_create(path->n);
    pnl_vect_clone(copyDelta, deltas);

    double multiple = (exp(-((option_->T_ - t)/(double)model_->numberOfDaysInOneYear_) * option_->domesticInterestRate_)) / (2 * model_->nbSample_ * fdStep_);
    PnlVect *multiplier = pnl_vect_create_from_scalar(path->n, multiple);
    PnlVect *inversed = pnl_vect_create_from_scalar(path->n, 1);

    double (*invFunction)(double);
    invFunction = Inverse;

    PnlVect *spot = pnl_vect_create(path->n);

    int i = 0;
    while (i < model_->assets_.size()) {
        // update the asset vector spots
        pnl_vect_set(spot, i, model_->assets_[i].spot_);
        i++;
    }
    i = 0;
    while (i < model_->currencies_.size() - 1) {
        // update the currencies vector spots
        pnl_vect_set(spot, i + model_->assets_.size() , model_->currencies_[i+1].spot_);
        i++;
    }

    pnl_vect_map(inversed, spot, invFunction);
    pnl_vect_mult_vect_term(deltas, multiplier);
    pnl_vect_mult_vect_term(deltas, inversed);

    // Setp up the deltasStdDev vector
    multiple = (exp(-2 * ((option_->T_ - t)/(double)model_->numberOfDaysInOneYear_) * option_->domesticInterestRate_)) / (4 * model_->nbSample_ * fdStep_* fdStep_);
    pnl_vect_set_all(multiplier, multiple);
    double (*squareInvFunction)(double);
    squareInvFunction = SquareInverse;
    pnl_vect_map(inversed, spot, squareInvFunction);
    pnl_vect_mult_vect_term(multiplier, inversed);

    pnl_vect_mult_vect_term(copyDelta, copyDelta);
    pnl_vect_mult_scalar(copyDelta, (double)1 / model_->nbSample_);
    pnl_vect_minus_vect(deltasStdDev, copyDelta);
    pnl_vect_mult_vect_term(deltasStdDev, multiplier);

    double (*squareRootFunction)(double);
    squareRootFunction = SquareRoot;

    // Take the square root of each variance to get the stdDev
    pnl_vect_map(deltasStdDev, deltasStdDev, squareRootFunction);
    pnl_vect_div_double(deltasStdDev, sqrt(model_->nbSample_));

    pnl_vect_free(&copyDelta);
    pnl_vect_free(&multiplier); 
    pnl_vect_free(&inversed);
    pnl_vect_free(&spot);
}