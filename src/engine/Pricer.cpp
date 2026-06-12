#include "Pricer.h"
#include "instruments/Option.h"
#include "instruments/Types.h"
#include "market/Market.h"
#include <cmath>
#include <stdexcept>
#include <vector>

namespace {
// Standard Normal CDF: N(x)
double norm_cdf(double x) { return 0.5 * std::erfc(-x / std::sqrt(2.0)); }
} // namespace

double BlackScholesOptionPricer::calculatePrice(const Market &market,
                                                const Option &option) const {
  if (option.getOptionExerciseStyle() == OptionExerciseStyle::American) {
    throw std::invalid_argument(
        "Error: Black-Scholes model only suppport Europrean options");
  }

  double S{market.getMarketData<StockPrice>(option.getUnderlyingName())};
  double vol{
      market.getMarketData<VolCurve>("ATM").getVol(option.getExpiryDate())};
  double r{market.getRateCurve(option.getTradeCcy())
               .getRate(option.getExpiryDate())};
  double K{option.getStrike()};
  double T{option.getExpiryDate() - market.getCurrentDate()};

  if (T <= 0.0) {
    throw std::invalid_argument("Error: Option has expired");
  }

  double d1 =
      (std::log(S / K) + (r + 0.5 * vol * vol) * T) / (vol * std::sqrt(T));
  double d2 = d1 - vol * std::sqrt(T);

  if (option.getOptionRight() == OptionRight::Call) {
    return S * norm_cdf(d1) - K * std::exp(-r * T) * norm_cdf(d2);
  } else {
    return K * std::exp(-r * T) * norm_cdf(-d2) - S * norm_cdf(-d1);
  }
}

double CRRBinTreeOptionPricer::calculatePrice(const Market &market,
                                              const Option &option) const {
  double S0{market.getMarketData<StockPrice>(option.getUnderlyingName())};
  const VolCurve &volCurve{market.getMarketData<VolCurve>("ATM")};
  const RateCurve &rateCurve{market.getRateCurve(option.getTradeCcy())};
  Date asOf{market.getCurrentDate()};

  double T{option.getExpiryDate() - asOf};

  if (T <= 0.0) {
    throw std::invalid_argument("Error: Option has expired");
  }

  double dt{T / m_timeSteps};

  std::vector<double> optionValues(static_cast<size_t>(m_timeSteps + 1));

  // Pre-calculate interpolated discount factors and variances for each time
  // step
  std::vector<double> dfs(static_cast<size_t>(m_timeSteps + 1));
  std::vector<double> vars(static_cast<size_t>(m_timeSteps + 1));

  for (int i{0}; i <= m_timeSteps; ++i) {
    Date t_i{asOf + (i * dt)};
    dfs[static_cast<size_t>(i)] = rateCurve.getDf(asOf, t_i);

    double vol_i{volCurve.getVol(t_i)};
    vars[static_cast<size_t>(i)] =
        // Total accumulated variance; Variance is additive
        vol_i * vol_i * (i * dt);
  }

  // Lambda to get the exact spot price at any node
  auto getSpot = [&](int step, int i) {
    if (step == 0)
      return S0;
    double jump{std::sqrt(vars[static_cast<size_t>(step)] / step)};
    return S0 * std::exp((2.0 * i - step) * jump);
  };

  // Get option value at maturity
  for (int i{0}; i <= m_timeSteps; ++i) {
    optionValues[static_cast<size_t>(i)] =
        option.payoff(getSpot(m_timeSteps, i));
  }

  // Backward induction
  for (int step{m_timeSteps - 1}; step >= 0; --step) {
    // DF(t1, t2) = DF(t2) / DF(t1)
    double fwdDiscountFactor =
        dfs[static_cast<size_t>(step + 1)] / dfs[static_cast<size_t>(step)];

    for (int i{0}; i <= step; ++i) {
      double currentSpot_step = getSpot(step, i);

      // Node-specific risk-neutral probability to match the forward rate
      double spot_up = getSpot(step + 1, i + 1);
      double spot_down = getSpot(step + 1, i);
      double a = currentSpot_step / fwdDiscountFactor;
      double p = (a - spot_down) / (spot_up - spot_down);

      // Ensure probability stay within [0.0, 1.0]
      p = std::max(0.0, std::min(1.0, p));

      double continuationValue{
          fwdDiscountFactor *
          (p * optionValues[static_cast<size_t>(i + 1)] +
           ((1.0 - p) * optionValues[static_cast<size_t>(i)]))};

      if (option.getOptionExerciseStyle() == OptionExerciseStyle::American) {
        double intrinsicValue{option.payoff(currentSpot_step)};
        optionValues[static_cast<size_t>(i)] =
            std::max(continuationValue, intrinsicValue);
      } else {
        optionValues[static_cast<size_t>(i)] = continuationValue;
      }
    }
  }

  return optionValues[0];
}

double JRBinTreeOptionPricer::calculatePrice(const Market &market,
                                             const Option &option) const {
  double S0{market.getMarketData<StockPrice>(option.getUnderlyingName())};
  const VolCurve &volCurve{market.getMarketData<VolCurve>("ATM")};
  const RateCurve &rateCurve{market.getRateCurve(option.getTradeCcy())};
  Date asOf{market.getCurrentDate()};

  double T{option.getExpiryDate() - asOf};

  if (T <= 0.0) {
    throw std::invalid_argument("Error: Option has expired");
  }

  double dt{T / m_timeSteps};

  std::vector<double> optionValues(static_cast<size_t>(m_timeSteps + 1));

  std::vector<double> dfs(static_cast<size_t>(m_timeSteps + 1));
  std::vector<double> vars(static_cast<size_t>(m_timeSteps + 1));

  // Pre-calculate interpolated discount factors and variances for each time
  // step
  for (int i{0}; i <= m_timeSteps; ++i) {
    Date t_i{asOf + (i * dt)};
    dfs[static_cast<size_t>(i)] = rateCurve.getDf(asOf, t_i);

    double vol_i{volCurve.getVol(t_i)};
    // Total accumulated variance; Variance is additive
    vars[static_cast<size_t>(i)] = vol_i * vol_i * (i * dt);
  }

  // JR requires p=0.5, so the spot price absorbs both the forward rate and
  // exact forward variance
  // Lambda to get the exact spot price at any node
  auto getSpot = [&](int step, int i) {
    if (step == 0)
      return S0;
    double V_t = vars[static_cast<size_t>(step)];
    double fwdPrice = S0 / dfs[static_cast<size_t>(step)];

    double jump = std::sqrt(V_t / step);
    return fwdPrice * std::exp(-0.5 * V_t + (2.0 * i - step) * jump);
  };

  // Get option value at maturity
  for (int i{0}; i <= m_timeSteps; ++i) {
    optionValues[static_cast<size_t>(i)] =
        option.payoff(getSpot(m_timeSteps, i));
  }

  double p{0.5};

  // Backward induction
  for (int step{m_timeSteps - 1}; step >= 0; --step) {
    double discountFactor =
        dfs[static_cast<size_t>(step + 1)] / dfs[static_cast<size_t>(step)];

    for (int i{0}; i <= step; ++i) {
      double continuationValue{
          discountFactor *
          (p * optionValues[static_cast<size_t>(i + 1)] +
           ((1.0 - p) * optionValues[static_cast<size_t>(i)]))};

      if (option.getOptionExerciseStyle() == OptionExerciseStyle::American) {
        double intrinsicValue{option.payoff(getSpot(step, i))};
        optionValues[static_cast<size_t>(i)] =
            std::max(continuationValue, intrinsicValue);
      } else {
        optionValues[static_cast<size_t>(i)] = continuationValue;
      }
    }
  }

  return optionValues[0];
}
