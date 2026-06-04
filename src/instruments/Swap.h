#ifndef SWAP_H
#define SWAP_H

#include "instruments/Trade.h"
#include "instruments/Types.h"
#include "market/Market.h"

class Swap final : public ITrade {
public:
  Swap(Date tradeDate, Date startDate, Date endDate, double notional,
       double fixedRate, double frequency)
      : ITrade{TradeType::Swap, tradeDate}, m_startDate{startDate},
        m_endDate{endDate}, m_notional{notional}, m_fixedRate{fixedRate},
        m_yearFreq{frequency} {}

  Date getStartDate() const { return m_startDate; }
  Date getEndDate() const { return m_endDate; }
  double getNotional() const { return m_notional; }
  double getFixedRate() const { return m_fixedRate; }
  double getFrequency() const { return m_yearFreq; }

  double presentValue(Market &market) const override {
    const double annuity{getAnnuity(market)};
    // Use maturity date's zero rate as the approximation of the par swap rate
    //
    // Par swap rate is a weighted average of the floating rate and we know from
    // no-arbitrage principles, the zero rate is also the average of short-term
    // interest rates
    const double parRate{
        market.getMarketData<RateCurve>("USD-SOFR").getRate(m_endDate)};

    return annuity * (m_fixedRate - parRate);
  };

  std::ostream &print(std::ostream &os) const override {
    os << "Bond object [TradeType: " << getTradeType()
       << ", TradeDate: " << getTradeDate() << ", Notional: " << m_notional
       << ", FixedRate: " << m_fixedRate << ", Frequency: " << m_yearFreq
       << "]\n";
    return os;
  }

private:
  Date m_startDate{};
  Date m_endDate{};
  double m_notional{};
  double m_fixedRate{};
  double m_yearFreq{};

  double getAnnuity(Market &market) const {
    double annuity{};
    const RateCurve &irCurve{market.getMarketData<RateCurve>("USD-SOFR")};
    Date paymentDate{m_endDate};

    while (paymentDate > m_startDate) {
      double T{paymentDate - market.getCurrentDate()};
      double zeroRate{irCurve.getRate(paymentDate)};
      double discountFactor{std::exp(-zeroRate * T)};

      double actualYearFrac{m_yearFreq};
      if ((paymentDate - m_startDate) < m_yearFreq) {
        actualYearFrac = paymentDate - m_startDate;
      }

      double notionalInPeriod{m_notional * actualYearFrac};
      annuity += notionalInPeriod * discountFactor;

      paymentDate -= m_yearFreq;
    }
    return annuity;
  }
};
#endif
