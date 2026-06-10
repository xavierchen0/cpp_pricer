#include <iostream>

#include "core/Date.h"
#include "market/Loaders.h"
#include "market/Market.h"

int main() {
  // task 1, create an market data object, and update the market data from from
  // txt file
  Date valueDate{2023, 12, 31};
  Market &market{Market::getInstance()};
  market.setCurrentDate(valueDate);

  try {
    // Load market data
    loadRateCurve("data/curve.txt");
    loadVolCurve("data/vol.txt", "SP500");
    loadStockPrices("data/stockPrice.txt");
    loadBondPrices("data/bondPrice.txt");

    market.display();
  } catch (const std::exception &e) {
    std::cerr << "Failed to load market data: " << e.what() << '\n';
    return 1;
  }

  // task 2, create a portfolio of bond, swap, european option, american option
  // for each time, at least should have long / short, different tenor or
  // expiry, different underlying totally no less than 16 trades
  std::vector<std::unique_ptr<ITrade>> portfolio;
  try {
    portfolio = loadTrades("data/trade.txt");
    std::cout << "\n--- Task 2: Portfolio Loaded ---\n";
    std::cout << "Successfully loaded " << portfolio.size() << " trades.\n";
    for (const auto &trade : portfolio) {
      std::cout << *trade;
    }
  } catch (const std::exception &e) {
    std::cerr << "Failed to load portfolio: " << e.what() << '\n';
    return 1;
  }

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
