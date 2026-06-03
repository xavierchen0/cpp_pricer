#ifndef SWAP_H
#define SWAP_H

#include "Market.h"
#include "Trade.h"
#include "Types.h"

class Swap final : public Trade {
public:
  // Does not make sense for object to be default initialised
  Swap() = delete;

  // Remove std::move because Date object only holds 3 int member variables
  // which is 12 bytes in most systems, and std::move on primitive types is
  // equivalent to just copying the data as there is no external heap memory to
  // transfer ownership of. For a small Date object, const ref is slower for the
  // CPU because of the additional step to dereference the object first before
  // fetching the data. In other words, the CPU has to read and look up the
  // memory address (dereference) before it can fetch the object, instead of
  // directly fetching the object.
  Swap(Date tradeDate, Date startDate, Date endDate, double notional,
       double fixedRate, double frequency)
      : Trade{TradeType::Swap, tradeDate}, m_startDate{startDate},
        m_endDate{endDate}, m_notional{notional}, m_fixedRate{fixedRate},
        m_yearFreq{frequency} {}

  Date getStartDate() const { return m_startDate; }
  Date getEndDate() const { return m_endDate; }
  double getNotional() const { return m_notional; }
  double getFixedRate() const { return m_fixedRate; }
  double getFrequency() const { return m_yearFreq; }

  double PV(const Market &market) const override {
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

private:
  Date m_startDate{};
  Date m_endDate{};
  double m_notional{};
  double m_fixedRate{};
  double m_yearFreq{};

  double getAnnuity(const Market &market) const {
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
