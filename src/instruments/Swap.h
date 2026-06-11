#ifndef SWAP_H
#define SWAP_H

#include "instruments/Trade.h"
#include "instruments/Types.h"
#include "market/Market.h"

class Swap final : public ITrade {
public:
  Swap(Date tradeDate, Date startDate, Date endDate, Currency tradeCcy,
       double notional, double fixedRate, double frequency)
      : ITrade{TradeType::Swap, tradeDate, tradeCcy}, m_startDate{startDate},
        m_endDate{endDate}, m_notional{notional}, m_fixedRate{fixedRate},
        m_yearFreq{frequency} {}

  Date getStartDate() const { return m_startDate; }
  Date getEndDate() const { return m_endDate; }
  double getNotional() const { return m_notional; }
  double getFixedRate() const { return m_fixedRate; }
  double getFrequency() const { return m_yearFreq; }

  double presentValue(const Market &market) const override {
    double pvFixed{0.0};
    double pvFloat{0.0};

    const RateCurve &irCurve{market.getRateCurve(getTradeCcy())};
    Date paymentDate{m_endDate};

    while (paymentDate > m_startDate) {
      double actualYearFrac{m_yearFreq};
      Date periodStartDate{paymentDate};
      periodStartDate -= m_yearFreq;

      // Handle the case of "stub" periods
      if (periodStartDate < m_startDate) {
        actualYearFrac = paymentDate - m_startDate;
        periodStartDate = m_startDate;
      }

      // T2 is payment date
      double T2{paymentDate - market.getCurrentDate()};
      double zeroRate2{irCurve.getRate(paymentDate)};
      double df2{std::exp(-zeroRate2 * T2)};

      // T1 is period start date
      double T1{periodStartDate - market.getCurrentDate()};
      double df1{1.0};
      if (T1 > 0) {
        double zeroRate1{irCurve.getRate(periodStartDate)};
        df1 = std::exp(-zeroRate1 * T1);
      }

      // Calculate forward rate
      double fwdRate{(df1 / df2 - 1.0) / actualYearFrac};

      // Fixed cash flow
      double fixedCashFlow{m_fixedRate * m_notional * actualYearFrac};
      pvFixed += fixedCashFlow * df2;

      // Floating cash flow
      double floatCashFlow{fwdRate * m_notional * actualYearFrac};
      pvFloat += floatCashFlow * df2;

      paymentDate = periodStartDate;
    }

    // Assuming positive notional means Receive Fixed, Pay Float
    return pvFixed - pvFloat;
  }

  std::ostream &print(std::ostream &os) const override {
    os << "Swap object [TradeType: " << getTradeType()
       << ", TradeDate: " << getTradeDate() << ", TradeCcy: " << getTradeCcy()
       << ", Notional: " << m_notional << ", FixedRate: " << m_fixedRate
       << ", Frequency: " << m_yearFreq << "]\n";
    return os;
  }

private:
  Date m_startDate{};
  Date m_endDate{};
  double m_notional{};
  double m_fixedRate{};
  double m_yearFreq{};
};
#endif
