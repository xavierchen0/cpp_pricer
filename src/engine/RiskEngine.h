#ifndef RISK_ENGINE_H
#define RISK_ENGINE_H

#include "instruments/Trade.h"
#include "market/Market.h"
#include <memory>
#include <vector>

struct RiskResult {
  double pv{0.0};
  double delta{0.0}; // IR Delta
  double vega{0.0};  // Vol Vega
};

class RiskEngine {
public:
  static std::vector<RiskResult>
  computeRisk(const std::vector<std::unique_ptr<ITrade>> &portfolio,
              const Market &baseMarket, bool useMultiThreading = true);
};

#endif
