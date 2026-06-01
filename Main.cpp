#include <iostream>

#include "Date.h"
#include "Market.h"
#include "MarketDataLoader.h"

int main() {
  // task 1, create an market data object, and update the market data from from
  // txt file
  Date valueDate{2023, 12, 31};

  Market market{valueDate};

  try {
    // Load market data
    loadRateCurve(market, "curve.txt");
    loadVolCurve(market, "vol.txt", "VolData");
    loadStockPrices(market, "stockPrice.txt");
    loadBondPrices(market, "bondPrice.txt");

    market.display();
  } catch (const std::exception &e) {
    std::cerr << "Failed to load market data: " << e.what() << '\n';
    return 1;
  }

  // task 2, create a portfolio of bond, swap, european option, american option
  // for each time, at least should have long / short, different tenor or
  // expiry, different underlying totally no less than 16 trades
  // std::vector<Trade *> myPortfolio;
  // Date tradeDate = valueDate;
  // Date startDate = Date(2023, 12, 31);
  // Date expiryDate = Date(2025, 12, 31);
  // Trade *swapTrade =
  //     new Swap(tradeDate, startDate, expiryDate, 1000000, 0.05, 2);
  // Trade *bondTrade =
  //     new Bond(tradeDate, "testBond1", startDate, expiryDate, 100000, 0.025,
  //     1);
  // Trade *eCallOption =
  //     new EuropeanOption(tradeDate, 10000, 100, expiryDate,
  //     OptionType::Call);
  // Trade *aOption =
  //     new AmericanOption(tradeDate, 10000, 100, expiryDate,
  //     OptionType::Call);
  //
  // myPortfolio.push_back(swapTrade);
  // myPortfolio.push_back(bondTrade);
  // myPortfolio.push_back(eCallOption);
  // myPortfolio.push_back(aOption);
  //
  // // task 3, creat a pricer and price the portfolio, output the pricing
  // result
  // // of each deal.
  // auto pricer = CRRBinomialTreePricer(20);
  // for (auto &trade : myPortfolio) {
  //   double pv = pricer.Price(*mkt, trade);
  //   // log pv details out in a file
  // }
  //
  // // task 4, analyzing pricing result
  // //  a) compare CRR binomial tree result for an european option vs Black
  // model
  // //  b) compare CRR binomial tree result for an american option call vs
  // //  european option call, and put
  //
  // // final
  // for (auto &trade : myPortfolio) {
  //   delete trade;
  // }
  //
  // delete mkt;
  //
  // std::cout << "Project build successfully!" << std::endl;
  // return 0;
}
