#include "Pricer.h"
#include <cmath>

double Pricer::Price(const Market &mkt, Trade *trade) {
  double pv = 0;
  if (trade->getType() == "Option") {
    Option *treePtr = dynamic_cast<Option *>(trade);
    if (treePtr) { // check if cast is sucessful
      pv = PriceTree(mkt, *treePtr);
    }
  } else {
    double price = 0; // get from market data
    pv = trade->Payoff(price);
  }

  return pv;
}

void BinomialTreePricer::ModelSetup(double S0, double sigma, double r,
                                    double dt) {
  // a basic version of binomial tree
  u = 1.1;
  d = 0.9;
  p = (exp(r) - d) / (u - d);
  currentSpot = S0;
}

double BinomialTreePricer::PriceTree(const Market &mkt, const Option &trade) {
  // model setup
  double T = trade.GetExpiry() - mkt.asOf;
  double dt = T / nTimeSteps;
  double stockPrice = 0, vol = 0, rate = 0;
  /*
  get these data for the deal from market object
  */
  ModelSetup(stockPrice, vol, rate, dt);

  // initialize
  for (int i = 0; i <= nTimeSteps; i++) {
    states[i] = trade.Payoff(GetSpot(nTimeSteps, i));
  }

  // price by backward induction
  for (int k = nTimeSteps - 1; k >= 0; k--)
    for (int i = 0; i <= k; i++) {
      // calculate continuation value
      double df = exp(-rate * dt);
      double continuation =
          df * (states[i] * GetProbUp() + states[i + 1] * GetProbDown());
      // calculate the option value at node(k, i)
      states[i] = trade.ValueAtNode(GetSpot(k, i), dt * k, continuation);
    }

  return states[0];
}

void CRRBinomialTreePricer::ModelSetup(double S0, double sigma, double rate,
                                       double dt) {
  // double b = std::exp((2 * rate + sigma * sigma) * dt) + 1;
  // u = (b + std::sqrt(b * b - 4 * std::exp(2 * rate * dt))) / 2 /
  // std::exp(rate * dt);
  u = exp(sigma * dt);
  d = exp(-sigma * dt);
  p = (exp(sigma * dt) - d) / (u - d);
  currentSpot = S0;
}

void JRRNBinomialTreePricer::ModelSetup(double S0, double sigma, double rate,
                                        double dt) {
  u = std::exp((rate - sigma * sigma / 2) * dt + sigma * std::sqrt(dt));
  d = std::exp((rate - sigma * sigma / 2) * dt - sigma * std::sqrt(dt));
  p = (std::exp(rate * dt) - d) / (u - d);
  currentSpot = S0;
}
