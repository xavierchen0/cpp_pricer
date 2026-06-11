#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "core/Date.h"
#include "engine/Pricer.h"
#include "engine/RiskEngine.h"
#include "instruments/Option.h"
#include "market/Loaders.h"
#include "market/Market.h"

int main() {
  // task 1, create an market data object, and update the market data from from
  // txt file
  Date valueDate{2023, 12, 31};
  Market market{};
  market.setCurrentDate(valueDate);

  try {
    // Load market data
    loadRateCurve("data/usd_curve.txt", market);
    loadRateCurve("data/sgd_curve.txt", market);
    loadVolCurve("data/vol.txt", "ATM", market);
    loadStockPrices("data/stockPrice.txt", market);

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
      std::cout << "  [" << i++ << "] " << *trade << '\n';
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

  // Task 5: Risk Engine & Performance Benchmarking
  std::cout << "==================================================\n";
  std::cout << " RISK ENGINE & PERFORMANCE BENCHMARKING \n";
  std::cout << "==================================================\n";

  // Set the default pricer for options so the risk engine uses it consistently
  for (const auto &trade : portfolio) {
    if (trade->getTradeType() == TradeType::Option) {
      auto *optionTrade{static_cast<Option *>(trade.get())};
      optionTrade->setPricer(pricers[1]); // CRR Tree
    }
  }

  std::cout << " Running Multi-threaded Risk Engine...\n";
  auto startMulti{std::chrono::high_resolution_clock::now()};
  std::vector<RiskResult> riskResultsMulti{
      RiskEngine::computeRisk(portfolio, market, true)};
  auto endMulti{std::chrono::high_resolution_clock::now()};
  std::chrono::duration<double, std::milli> msDoubleMulti{endMulti -
                                                          startMulti};

  std::cout << " Running Single-threaded Risk Engine...\n";
  auto startSingle{std::chrono::high_resolution_clock::now()};
  std::vector<RiskResult> riskResultsSingle{
      RiskEngine::computeRisk(portfolio, market, false)};
  auto endSingle{std::chrono::high_resolution_clock::now()};
  std::chrono::duration<double, std::milli> msDoubleSingle{endSingle -
                                                           startSingle};

  std::cout << "\n [Performance Analysis]\n";
  std::cout << "   - Multi-threaded time:  " << std::fixed
            << std::setprecision(2) << msDoubleMulti.count() << " ms\n";
  std::cout << "   - Single-threaded time: " << std::fixed
            << std::setprecision(2) << msDoubleSingle.count() << " ms\n";
  if (msDoubleMulti.count() > 0) {
    std::cout << "   -> Speedup Factor:      " << std::fixed
              << std::setprecision(2)
              << msDoubleSingle.count() / msDoubleMulti.count() << "x\n\n";
  }

  // Use Multi-threaded results for output
  std::vector<RiskResult> &riskResults{riskResultsMulti};

  double totalDelta{0.0};
  double totalVega{0.0};

  for (size_t k{0}; k < portfolio.size(); ++k) {
    std::cout << "  [" << k + 1 << "] " << *portfolio[k];
    std::cout << "      => PV:    " << std::fixed << std::setprecision(2)
              << std::setw(10) << riskResults[k].pv << "\n";
    std::cout << "      => Delta: " << std::fixed << std::setprecision(2)
              << std::setw(10) << riskResults[k].delta << "\n";
    std::cout << "      => Vega:  " << std::fixed << std::setprecision(2)
              << std::setw(10) << riskResults[k].vega << "\n\n";

    totalDelta += riskResults[k].delta;
    totalVega += riskResults[k].vega;
  }

  std::cout << "--------------------------------------------------\n";
  std::cout << " TOTAL PORTFOLIO DELTA: " << std::fixed << std::setprecision(2)
            << totalDelta << "\n";
  std::cout << " TOTAL PORTFOLIO VEGA:  " << std::fixed << std::setprecision(2)
            << totalVega << "\n";
  std::cout << "==================================================\n\n";

  return 0;
}
