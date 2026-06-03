#ifndef _AMERICAN_TRADE
#define _AMERICAN_TRADE

#include <cassert>

#include "Payoff.h"
#include "Types.h"
#include "tradeOptionProduct.h"

class AmericanOption : public Option {
public:
  AmericanOption(const Date &tradeDate, double _notional, double _strike,
                 const Date &_expiry, OptionType _optType)
      : Option(tradeDate, _optType), strike(_strike), notional(_notional),
        expiryDate(_expiry) {}
  virtual double Payoff(double S) const {
    return PAYOFF::VanillaOption(optType, strike, S);
  }
  virtual const Date &GetExpiry() const { return expiryDate; }
  virtual double ValueAtNode(double S, double t, double continuation) const {
    return std::max(Payoff(S), continuation);
  }
  inline void setStrike(double _strike) { strike = _strike; };
  inline void setOptionType(OptionType type) { optType = type; };

private:
  double strike;
  double notional;
  Date expiryDate;
};

class AmerCallSpread : public Option {
public:
  AmerCallSpread(const Date &tradeDate, double _notional, double _k1,
                 double _k2, const Date &_expiry)
      : Option(tradeDate, OptionType::Call), strike1(_k1), strike2(_k2),
        notional(_notional), expiryDate(_expiry) {
    assert(_k1 < _k2);
  }
  virtual double Payoff(double S) const {
    return PAYOFF::CallSpread(strike1, strike2, S);
  }
  virtual const Date &GetExpiry() const { return expiryDate; }

private:
  double strike1;
  double strike2;
  double notional;
  Date expiryDate;
};

#endif
