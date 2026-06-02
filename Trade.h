#ifndef TRADE_H
#define TRADE_H

#include "Date.h"
#include "Market.h"
#include "Types.h"
#include <ostream>
#include <utility>

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
  Trade(TradeType type, Date date)
      : m_tradeType{type}, m_tradeDate{std::move(date)} {};

  // Allow derived classes to inherit these member variables
  TradeType m_tradeType{};
  Date m_tradeDate{};
};

#endif
