#ifndef _TREE_PRODUCT_H
#define _TREE_PRODUCT_H

#include "core/Date.h"
#include "instruments/Trade.h"
#include "instruments/Types.h"

// This class provide a common member function interface for option type of
// trade.
class Option : public ITrade {
public:
  Option(Date tradeDate, Date expiryDate, OptionRight optionRight,
         std::string underlyingName, Currency tradeCcy)
      : ITrade{TradeType::Option, tradeDate, tradeCcy},
        m_optionRight{optionRight}, m_underlyingName{std::move(underlyingName)},
        m_expiryDate{expiryDate} {}

  OptionRight getOptionRight() const { return m_optionRight; }
  const std::string &getUnderlyingName() const { return m_underlyingName; }
  Date getExpiryDate() const { return m_expiryDate; }

  virtual double payoff(double underlyingSpotPrice) const = 0;

  virtual OptionExerciseStyle getExerciseStyle() const = 0;

private:
  OptionRight m_optionRight{};
  std::string m_underlyingName{};
  Date m_expiryDate{};
};

#endif
