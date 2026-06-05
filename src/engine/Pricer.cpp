#include "Pricer.h"
#include "instruments/Option.h"
#include "instruments/Types.h"
#include "market/Market.h"
#include <cmath>
#include <vector>

double CRRBinTreeOptionPricer::calculatePrice(const Market &market,
                                              const Option &option) const {
  // Market Parameters
  double S0{market.getMarketData<StockPrice>(option.getUnderlyingName())};
  double vol{market.getMarketData<VolCurve>(option.getUnderlyingName())
                 .getVol(option.getExpiryDate())};
  double r{market.getRateCurve(option.getTradeCcy())
               .getRate(option.getExpiryDate())}; // risk-free rate

  double T{option.getExpiryDate() - market.getCurrentDate()};
  double dt{T / m_timeSteps};

  // Bin Tree parameters
  double u{std::exp(vol * std::sqrt(dt))};
  double d{1.0 / u};
  double a{std::exp(r * dt)};
  double p{(a - d) / (u - d)};
  double discountFactor{std::exp(-r * dt)};

  /*
  Instead of a 2D matrix (stock price vs time), we can just use a 1D vector
  to hold the previous time step option prices during backward induction. This
  can help us to save space because once an option value of the previous time
  step has been used, it won't be used again and can be safely overwritten.
  */
  std::vector<double> optionValues(static_cast<size_t>(m_timeSteps + 1));

  // Calculate payoff at maturity
  // Start with bottom node because the loop below starts with bottom node
  double currentSpot{S0 * std::pow(d, m_timeSteps)};
  double upDownratio{u * u}; // = u / d = u / 1 / u = u * u
  for (int i{0}; i <= m_timeSteps; ++i) {
    // i represent the number of up moves; (m_timSteps - i) is the number of
    // down moves
    optionValues[static_cast<size_t>(i)] = option.payoff(currentSpot);
    currentSpot *= upDownratio;
  }

  // Backward induction
  // Start from one step before the last time step
  for (int step{m_timeSteps - 1}; step >= 0; --step) {
    double currentSpot{S0 * std::pow(d, step)};

    for (int i{0}; i <= step; ++i) {
      double continuationValue{
          discountFactor *
          (p * optionValues[static_cast<size_t>(i + 1)] +
           ((1.0 - p) * optionValues[static_cast<size_t>(i)]))};

      if (option.getOptionExerciseStyle() == OptionExerciseStyle::American) {
        double intrinsicValue{option.payoff(currentSpot)};

        optionValues[static_cast<size_t>(i)] =
            std::max(continuationValue, intrinsicValue);
      } else {
        optionValues[static_cast<size_t>(i)] = continuationValue;
      }

      currentSpot *= upDownratio;
    }
  }

  return optionValues[0];
}
