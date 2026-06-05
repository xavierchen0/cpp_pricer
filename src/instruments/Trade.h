#ifndef TRADE_H
#define TRADE_H

#include "core/Date.h"
#include "instruments/Types.h"
#include "market/Market.h"
#include <ostream>

class ITrade {
public:
  virtual ~ITrade() = default;

  TradeType getTradeType() const { return m_tradeType; }
  Date getTradeDate() const { return m_tradeDate; }
  Currency getTradeCcy() const { return m_tradeCcy; }

  virtual double presentValue(const Market &market) const = 0;

  friend std::ostream &operator<<(std::ostream &os, const ITrade &trade) {
    return trade.print(os);
  }

  virtual std::ostream &print(std::ostream &os) const {
    os << "Trade object [TradeType: " << m_tradeType
       << " , TradeDate: " << m_tradeDate << " , TradeCcy: " << m_tradeCcy
       << "]\n";
    return os;
  }

protected:
  // This is an abstract base class and should not be instantiated directly

  // Remove std::move because Date object only holds 3 int member variables
  // which is 12 bytes in most systems, and std::move on primitive types is
  // equivalent to just copying the data as there is no external heap memory to
  // transfer ownership of. For a small Date object, const ref is slower for the
  // CPU because of the additional step to dereference the object first before
  // fetching the data. In other words, the CPU has to read and look up the
  // memory address (dereference) before it can fetch the object, instead of
  // directly fetching the object.
  ITrade(TradeType tradeType, Date tradeDate, Currency tradeCcy)
      : m_tradeType{tradeType}, m_tradeDate{tradeDate}, m_tradeCcy{tradeCcy} {};

private:
  TradeType m_tradeType{};
  Date m_tradeDate{};
  Currency m_tradeCcy{};
};

#endif
