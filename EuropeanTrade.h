#ifndef _EUROPEAN_TRADE
#define _EUROPEAN_TRADE

#include <cassert>

#include "OptionProduct.h"
#include "Payoff.h"
#include "Types.h"

class EuropeanOption : public Option
{
public:

	EuropeanOption(const Date& tradeDate, double _notional, double _strike, const Date& _expiry, OptionType _optType) :
		Option(tradeDate, _optType),
		strike(_strike),
		notional(_notional),
		expiryDate(_expiry)
	{
	};

	double Payoff(double S) const { return PAYOFF::VanillaOption(optType, strike, S); }
	const Date& GetExpiry() const { return expiryDate; }
	double ValueAtNode(double S, double t, double continuation) const { return continuation; }

	inline void setStrike(double _strike)
	{
		strike = _strike;
	};
	inline void setOptionType(OptionType type)
	{
		optType = type;
	};

private:

	double strike;
	double notional;
	Date expiryDate;
};

class EuropeanCallSpread : public Option
{
public:
	EuropeanCallSpread(const Date& tradeDate, double _notional, double _k1, double _k2, const Date& _expiry) : 
		Option(tradeDate, OptionType::Call),
		strike1(_k1), 
		strike2(_k2),
		expiryDate(_expiry)
	{
		expiryDate = _expiry;
		assert(_k1 < _k2);
	};
	double Payoff(double S) const { return PAYOFF::CallSpread(strike1, strike2, S); };
	const Date& GetExpiry() const { return expiryDate; };

private:
	double strike1;
	double strike2;
	Date expiryDate;
};

#endif
