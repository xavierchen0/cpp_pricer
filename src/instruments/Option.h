#ifndef _TREE_PRODUCT_H
#define _TREE_PRODUCT_H

#include "core/Date.h"
#include "engine/Payoff.h"
#include "engine/Pricer.h"
#include "instruments/Trade.h"
#include "instruments/Types.h"
#include "market/Market.h"
#include <memory>
#include <stdexcept>

// This class provide a common member function interface for option type of
// trade.
class Option : public ITrade {
public:
  Option(Date tradeDate, Date expiryDate, Currency tradeCcy,
         std::string underlyingName, OptionRight optionRight,
         OptionExerciseStyle optionExerciseStyle,
         std::unique_ptr<IOptionPayoff> optionPayoff,
         std::unique_ptr<IOptionPricer> optionPricer, double strike)
      : ITrade{TradeType::Option, tradeDate, tradeCcy},
        m_optionRight{optionRight}, m_optionExerciseStyle{optionExerciseStyle},
        m_optionPayoff{std::move(optionPayoff)},
        m_optionPricer{std::move(optionPricer)},
        m_underlyingName{std::move(underlyingName)}, m_expiryDate{expiryDate},
        m_strike{strike} {
    if (m_strike <= 0) {
      throw std::invalid_argument(
          "Error: strike must be strictly more than zero");
    }
  }

  OptionRight getOptionRight() const { return m_optionRight; }
  OptionExerciseStyle getOptionExerciseStyle() const {
    return m_optionExerciseStyle;
  }
  const std::string &getUnderlyingName() const { return m_underlyingName; }
  Date getExpiryDate() const { return m_expiryDate; }
  double getStrike() const { return m_strike; }

  double payoff(double underlyingSpotPrice) const {
    return (*m_optionPayoff)(underlyingSpotPrice);
  };

  double presentValue(const Market &market) const {
    return m_optionPricer->calculatePrice(market, *this);
  }

private:
  OptionRight m_optionRight{};
  OptionExerciseStyle m_optionExerciseStyle{};
  std::unique_ptr<IOptionPayoff> m_optionPayoff{};
  std::unique_ptr<IOptionPricer> m_optionPricer{};
  std::string m_underlyingName{};
  Date m_expiryDate{};
  double m_strike{};
};

#endif
