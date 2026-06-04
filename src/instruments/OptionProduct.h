#ifndef _TREE_PRODUCT_H
#define _TREE_PRODUCT_H

#include "core/Date.h"
#include "instruments/Trade.h"
#include "instruments/Types.h"

// This class provide a common member function interface for option type of
// trade.
class IOption : public ITrade {
public:
  // Does not make sense for object to be default initialised
  IOption() = delete;

  virtual ~IOption() = default;

  OptionRight getOptionRight() const { return m_optionRight; }
  const std::string &getUnderlyingName() const { return m_underlyingName; }
  Date getExpiryDate() const { return m_expiryDate; }

  virtual double valueAtNode(double underlyingPrice, double timeStep,
                             double continuationValue) const = 0;

protected:
  // This is an abstract base class and should not be instantiated directly
  IOption(OptionRight optionRight, std::string underlyingName, Date tradeDate,
          Date expiryDate)
      : ITrade{TradeType::Option, tradeDate}, m_optionRight{optionRight},
        m_underlyingName{std::move(underlyingName)}, m_expiryDate{expiryDate} {}

private:
  OptionRight m_optionRight{};
  std::string m_underlyingName{};
  Date m_expiryDate{};
};

#endif
