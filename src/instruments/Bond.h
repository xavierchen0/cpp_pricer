#ifndef BOND_H
#define BOND_H

#include "core/Date.h"
#include "instruments/Trade.h"
#include "instruments/Types.h"
#include "market/Market.h"
#include <cmath>
#include <ostream>
#include <string>
#include <utility>

class Bond final : public Trade {
public:
  // Does not make sense for object to be default initialised
  Bond() = delete;

  // Remove std::move because Date object only holds 3 int member variables
  // which is 12 bytes in most systems, and std::move on primitive types is
  // equivalent to just copying the data as there is no external heap memory to
  // transfer ownership of. For a small Date object, const ref is slower for the
  // CPU because of the additional step to dereference the object first before
  // fetching the data. In other words, the CPU has to read and look up the
  // memory address (dereference) before it can fetch the object, instead of
  // directly fetching the object.
  Bond(std::string name, Date tradeDate, Date startDate, Date endDate,
       double notional, double couponRate, double frequency)
      : Trade{TradeType::Bond, tradeDate}, m_name{std::move(name)},
        m_startDate{startDate}, m_endDate{endDate}, m_notional{notional},
        m_couponRate{couponRate}, m_yearFreq{frequency} {}

  const std::string &getName() const { return m_name; }
  Date getStartDate() const { return m_startDate; }
  Date getEndDate() const { return m_endDate; }
  double getNotional() const { return m_notional; }
  double getCouponRate() const { return m_couponRate; }
  double getFrequency() const { return m_yearFreq; }

  double payoff(const Market &market) const override {
    // Calculate the theoretical PV rather than the MTM price

    // MTM approach
    // double bondPrice{market.getMarketData<BondPrice>(m_name) /
    //                  100.0}; // Bond prices quoted as percentages of par
    //                  value
    // return bondPrice * m_notional;

    // Theoretical PV by discounting future cash flows
    double bondPrice{};
    const RateCurve &irCurve{market.getMarketData<RateCurve>("USD-SOFR")};
    Date paymentDate{m_endDate};

    while (paymentDate > m_startDate) {
      double T{paymentDate - market.getCurrentDate()};
      double zeroRate{irCurve.getRate(paymentDate)};
      double discountFactor{std::exp(-zeroRate * T)};

      double actualYearFrac{m_yearFreq};
      // Handle the case of "stub" periods
      if ((paymentDate - m_startDate) < m_yearFreq) {
        actualYearFrac = paymentDate - m_startDate;
      }

      double couponAmt{m_couponRate * m_notional * actualYearFrac};
      bondPrice += couponAmt * discountFactor;

      // Pay the principal at maturity too
      if (paymentDate == m_endDate) {
        bondPrice += m_notional * discountFactor;
      }

      paymentDate -= m_yearFreq;
    }
    return bondPrice;
  }

  std::ostream &print(std::ostream &os) const override {
    os << "Bond object [TradeType: " << getTradeType()
       << ", TradeDate: " << getTradeDate() << ", Name: " << m_name
       << ", Notional: " << m_notional << ", Coupon Rate: " << m_couponRate
       << ", Frequency: " << m_yearFreq << "]\n";
    return os;
  }

private:
  std::string m_name{};
  Date m_startDate{};
  Date m_endDate{};
  double m_notional{};
  double m_couponRate{};
  double m_yearFreq{};
};

#endif
