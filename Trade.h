#ifndef TRADE_H
#define TRADE_H

#include "Date.h"
#include "Market.h"
#include "Types.h"
#include <ostream>

class Trade {
public:
  virtual ~Trade() = default;

  TradeType getType() const { return m_tradeType; }
  const Date &getTradeDate() const { return m_tradeDate; }

  virtual double PV(const Market &market) const = 0;

  friend std::ostream &operator<<(std::ostream &os, const Trade &trade) {
    return trade.print(os);
  }

  virtual std::ostream &print(std::ostream &os) const {
    os << "Trade object [TradeType: " << m_tradeType
       << " , TradeDate: " << m_tradeDate << "]\n";
    return os;
  }

protected:
  // This is an abstract base class and should not be instantiated directly
  Trade() = default;
  // Remove std::move because Date object only holds 3 int member variables
  // which is 12 bytes in most systems, and std::move on primitive types is
  // equivalent to just copying the data as there is no external heap memory to
  // transfer ownership of. For a small Date object, const ref is slower for the
  // CPU because of the additional step to dereference the object first before
  // fetching the data. In other words, the CPU has to read and look up the
  // memory address (dereference) before it can fetch the object, instead of
  // directly fetching the object.
  Trade(TradeType type, Date date) : m_tradeType{type}, m_tradeDate{date} {};

  // Allow derived classes to inherit these member variables
  TradeType m_tradeType{};
  Date m_tradeDate{};
};

#endif
