#include "engine/RiskEngine.h"
#include "instruments/Option.h"
#include <future>
#include <stdexcept>

namespace {
RiskResult compute(const ITrade *trade, const Market &baseMarket) {
  RiskResult result{};

  // Calculate base PV
  try {
    result.pv = trade->presentValue(baseMarket);
  } catch (const std::exception &e) {
    std::ostringstream oss;
    oss << "Error: Pricing failed for " << *trade << " | Reason: " << e.what();
    throw std::invalid_argument(oss.str());
  }

  // Calculate IR Delta with Central Difference
  double irBump{0.0001}; // 1bp == 0.0001

  Market marketIrUp{baseMarket};
  marketIrUp.bumpRateCurve(trade->getTradeCcy(), irBump);
  double pvIrUp{trade->presentValue(marketIrUp)};

  Market marketIrDown{baseMarket};
  marketIrDown.bumpRateCurve(trade->getTradeCcy(), -irBump);
  double pvIrDown{trade->presentValue(marketIrDown)};

  result.delta = (pvIrUp - pvIrDown) / (2.0 * irBump);

  // Calculate Vega with Central Difference
  // Only applied to Options
  if (trade->getTradeType() == TradeType::Option) {
    const Option *optionTrade{static_cast<const Option *>(trade)};
    double volBump{0.01}; // 1% == 0.01

    Market marketVolUp{baseMarket};
    marketVolUp.bumpVolCurve(optionTrade->getUnderlyingName(), volBump);
    double pvVolUp{optionTrade->presentValue(marketVolUp)};

    Market marketVolDown{baseMarket};
    marketVolDown.bumpVolCurve(optionTrade->getUnderlyingName(), -volBump);
    double pvVolDown{optionTrade->presentValue(marketVolDown)};

    result.vega = (pvVolUp - pvVolDown) / (2.0 * volBump);
  }

  return result;
}
} // namespace

std::vector<RiskResult>
RiskEngine::computeRisk(const std::vector<std::unique_ptr<ITrade>> &portfolio,
                        const Market &baseMarket, bool useMultiThreading) {
  std::vector<RiskResult> results(portfolio.size());

  if (useMultiThreading) {
    std::vector<std::future<RiskResult>> futures{};
    futures.reserve(portfolio.size());

    for (const auto &trade : portfolio) {
      futures.push_back(std::async(std::launch::async, compute, trade.get(),
                                   std::ref(baseMarket)));
    }

    // Wait and collect results
    for (size_t i = 0; i < futures.size(); ++i) {
      results[i] = futures[i].get();
    }
  } else {
    // Single threaded
    for (size_t i = 0; i < portfolio.size(); ++i) {
      results[i] = compute(portfolio[i].get(), baseMarket);
    }
  }

  return results;
}
