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
  double vol{
      market.getMarketData<VolCurve>("ATM").getVol(option.getExpiryDate())};
  const RateCurve &rateCurve{market.getRateCurve(option.getTradeCcy())};
  Date asOf{market.getCurrentDate()};

  double T{option.getExpiryDate() - asOf};

  if (T <= 0.0) {
    throw std::invalid_argument("Error: Option has expired");
  }

  double dt{T / m_timeSteps};

  double u{std::exp(vol * std::sqrt(dt))};
  double d{1.0 / u};

  std::vector<double> optionValues(static_cast<size_t>(m_timeSteps + 1));

  // Initialise array of discount factors from time step t_i to current date to
  // use as we iterate backwards
  std::vector<double> dfs(static_cast<size_t>(m_timeSteps + 1));
  for (int i{0}; i <= m_timeSteps; ++i) {
    Date t_i{asOf + (i * dt)};
    dfs[static_cast<size_t>(i)] = rateCurve.getDf(asOf, t_i);
  }

  // Find option prices at maturity across all paths
  double currentSpot{S0 * std::pow(d, m_timeSteps)};
  double upDownratio{u * u};
  for (int i{0}; i <= m_timeSteps; ++i) {
    optionValues[static_cast<size_t>(i)] = option.payoff(currentSpot);
    currentSpot *= upDownratio;
  }

  // Backwards induction
  for (int step{m_timeSteps - 1}; step >= 0; --step) {
    // DF(t1, t2) = DF(t2) / DF(t1); t1 < t2
    double fwdDiscountFactor =
        dfs[static_cast<size_t>(step + 1)] / dfs[static_cast<size_t>(step)];

    double a = 1.0 / fwdDiscountFactor; // equivalent to e^(r * dt)
    double p = (a - d) / (u - d);

    double currentSpot_step{S0 * std::pow(d, step)};

    for (int i{0}; i <= step; ++i) {
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

      currentSpot_step *= upDownratio;
    }
  }

  return optionValues[0];
}

double JRBinTreeOptionPricer::calculatePrice(const Market &market,
                                             const Option &option) const {
  double S0{market.getMarketData<StockPrice>(option.getUnderlyingName())};
  double vol{
      market.getMarketData<VolCurve>("ATM").getVol(option.getExpiryDate())};
  const RateCurve &rateCurve{market.getRateCurve(option.getTradeCcy())};
  Date asOf{market.getCurrentDate()};

  double T{option.getExpiryDate() - asOf};

  if (T <= 0.0) {
    throw std::invalid_argument("Error: Option has expired");
  }

  double dt{T / m_timeSteps};

  std::vector<double> optionValues(static_cast<size_t>(m_timeSteps + 1));

  // Initialise array of discount factors from time step t_i to current date to
  // use as we iterate backwards
  std::vector<double> dfs(static_cast<size_t>(m_timeSteps + 1));
  for (int i{0}; i <= m_timeSteps; ++i) {
    Date t_i{asOf + (i * dt)};
    dfs[static_cast<size_t>(i)] = rateCurve.getDf(asOf, t_i);
  }

  // Lambda to find spot price at node
  auto getSpot{[&](int step, int i) {
    double fwdPrice{S0 / dfs[static_cast<size_t>(step)]};
    return fwdPrice * std::exp(-0.5 * vol * vol * step * dt +
                               (2.0 * i - step) * vol * std::sqrt(dt));
  }};

  // Find option prices at maturity across all paths
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
