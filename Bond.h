#ifndef BOND_H
#define BOND_H

#include "Trade.h"
#include "Types.h"
#include <ostream>
#include <string>
#include <utility>

class Bond final : public Trade {
public:
  Bond(std::string name, Date tradeDate, Date startDate, Date endDate,
       double notional, double couponRate, int frequency)
      : Trade{TradeType::Bond, std::move(tradeDate)}, m_name{std::move(name)},
        m_startDate{std::move(startDate)}, m_endDate{std::move(endDate)},
        m_notional{notional}, m_couponRate{couponRate}, m_frequency{frequency} {
  }

  const std::string &getName() const { return m_name; }
  const Date &getStartDate() const { return m_startDate; }
  const Date &getEndDate() const { return m_endDate; }
  double getNotional() const { return m_notional; }
  double getCouponRate() const { return m_couponRate; }
  int getFrequency() const { return m_frequency; }

  double Payoff(double marketPrice) const override {
    return marketPrice * m_notional;
  }

  std::ostream &print(std::ostream &os) const override {
    os << "Bond object [TradeType: " << m_tradeType
       << ", TradeDate: " << m_tradeDate << ", Name: " << m_name
       << ", Notional: " << m_notional << ", Coupon Rate: " << m_couponRate
       << ", Frequency: " << m_frequency << "]\n";
    return os;
  }

private:
  std::string m_name{};
  Date m_startDate{};
  Date m_endDate{};
  double m_notional{};
  double m_couponRate{};
  int m_frequency{};
};

#endif
