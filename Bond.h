#ifndef BOND_H
#define BOND_H

#include "Market.h"
#include "Trade.h"
#include "Types.h"
#include <cmath>
#include <ostream>
#include <string>
#include <utility>

class Bond final : public Trade {
public:
  Bond(std::string name, Date tradeDate, Date startDate, Date endDate,
       double notional, double couponRate, double frequency)
      : Trade{TradeType::Bond, std::move(tradeDate)}, m_name{std::move(name)},
        m_startDate{std::move(startDate)}, m_endDate{std::move(endDate)},
        m_notional{notional}, m_couponRate{couponRate}, m_yearFreq{frequency} {}

  const std::string &getName() const { return m_name; }
  const Date &getStartDate() const { return m_startDate; }
  const Date &getEndDate() const { return m_endDate; }
  double getNotional() const { return m_notional; }
  double getCouponRate() const { return m_couponRate; }
  double getFrequency() const { return m_yearFreq; }

  double PV(const Market &market) const override {
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
    os << "Bond object [TradeType: " << m_tradeType
       << ", TradeDate: " << m_tradeDate << ", Name: " << m_name
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
