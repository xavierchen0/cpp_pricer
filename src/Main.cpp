#include <array>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "core/Date.h"
#include "engine/Pricer.h"
#include "instruments/Option.h"
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
    loadVolCurve("data/vol.txt", "TMP");
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
    std::cout << "\n==================================================\n";
    std::cout << " PORTFOLIO LOADED\n";
    std::cout << "==================================================\n";
    std::cout << " Successfully loaded " << portfolio.size() << " trades.\n";
    std::cout << "--------------------------------------------------\n";
    int i{1};
    for (const auto &trade : portfolio) {
      std::cout << "  [" << i++ << "] " << *trade;
    }
    std::cout << "==================================================\n";
  } catch (const std::exception &e) {
    std::cerr << "Failed to load portfolio: " << e.what() << '\n';
    return 1;
  }

  // task 3 & 4, create a pricer and price the portfolio, output the pricing
  // result of each deal, and run analysis for options.
  std::cout << "\n==================================================\n";
  std::cout << " PRICING PORTFOLIO & ANALYSIS\n";
  std::cout << "==================================================\n";

  // Initialize shared pricers once
  std::array<std::shared_ptr<IOptionPricer>, 3> pricers{
      std::make_shared<BlackScholesOptionPricer>(),
      std::make_shared<CRRBinTreeOptionPricer>(50), // 50 time steps
      std::make_shared<JRBinTreeOptionPricer>(50),
  };

  double totalPV{0.0};
  int i{1};
  for (const auto &trade : portfolio) {
    std::cout << "  [" << i++ << "] " << *trade;

    if (trade->getTradeType() != TradeType::Option) {
      try {
        double pv{trade->presentValue(market)};
        std::cout << "      => PV: " << std::fixed << std::setprecision(2) << pv
                  << "\n\n";
        totalPV += pv;
      } catch (const std::exception &e) {
        std::cout << "      => Error: " << e.what() << "\n\n";
      }
    } else { // Perform pricing analysis for options only
      auto *optionTrade = static_cast<Option *>(trade.get());
      std::cout << "      [Pricing Analysis]:\n";

      // 1. Black-Scholes
      try {
        optionTrade->setPricer(pricers[0]);
        double bsPV = optionTrade->presentValue(market);
        std::cout << "        - Black-Scholes PV: " << std::fixed
                  << std::setprecision(2) << bsPV << "\n";
      } catch (const std::exception &e) {
        std::cout << "        - Black-Scholes PV: [Error] " << e.what() << "\n";
      }

      // 2. CRR Binomial Tree
      try {
        optionTrade->setPricer(pricers[1]);
        double crrPV = optionTrade->presentValue(market);
        totalPV += crrPV; // Add crr PV to total for the sake of having a single
                          // totalPV
        std::cout << "        - CRR Tree PV:      " << std::fixed
                  << std::setprecision(2) << crrPV << "\n";
      } catch (const std::exception &e) {
        std::cout << "        - CRR Tree PV:      [Error] " << e.what() << "\n";
      }

      // 3. JR Binomial Tree
      try {
        optionTrade->setPricer(pricers[2]);
        double jrPV = optionTrade->presentValue(market);
        std::cout << "        - JR Tree PV:       " << std::fixed
                  << std::setprecision(2) << jrPV << "\n\n";
      } catch (const std::exception &e) {
        std::cout << "        - JR Tree PV:       [Error] " << e.what()
                  << "\n\n";
      }
    }
  }

  std::cout << "--------------------------------------------------\n";
  std::cout << " TOTAL PORTFOLIO PV: " << std::fixed << std::setprecision(2)
            << totalPV << "\n";
  std::cout << "==================================================\n\n";
  return 0;
}
