#ifndef _TREE_PRODUCT_H
#define _TREE_PRODUCT_H

#include "core/Date.h"
#include "instruments/Trade.h"
#include "instruments/Types.h"

// This class provide a common member function interface for option type of
// trade.
class Option : public Trade {
public:
  // Does not make sense for object to be default initialised
  Option() = delete;

  virtual ~Option() = default;

  OptionType getOptionType() const { return m_optType; }
  const std::string &getUnderlyingName() const { return m_underlyingName; }
  Date getExpiryDate() const { return m_expiryDate; }

  virtual double valueAtNode(double underlyingPrice, double timeStep,
                             double continuationValue) const = 0;

protected:
  // This is an abstract base class and should not be instantiated directly
  Option(OptionType optionType, std::string underlyingName, Date tradeDate,
         Date expiryDate)
      : Trade{TradeType::Option, tradeDate}, m_optType{optionType},
        m_underlyingName{std::move(underlyingName)}, m_expiryDate{expiryDate} {}

private:
  OptionType m_optType{};
  std::string m_underlyingName{};
  Date m_expiryDate{};
};

#endif
