#pragma once
#include "Trade.h"

class Swap : public Trade
{
public:
	Swap(const Date& start, const Date& end) : Trade("SwapTrade", start)
	{
		/*
		add constructor details
		*/
	}

	// make necessary change
	Swap(const Date& tradeDate, const Date& start, const Date& end, double notional, double rate, int freq) : Trade("SwapTrade", start)
	{
		/*
		add constructor details
		*/
	}
	inline double Payoff(double marketRate) const
	{
		/*
		Implement this, using npv = annuity * (traded rate - market swap rate);
		Annuity = sum of (notional * year fraction of each coupon period * Discount factor at each period end);
		Df = exp(-zT), z is the zero coupon rate;
		*/
		return 0;
	};

	void setFreq(double freq)
	{

		frequency = freq;
	}
	void setRate(double r)
	{

		tradeRate = r;
	}

	void setNotional(double n)
	{

		swapNotional = n;
	}

private:
	Date startDate;
	Date endDate;
	double swapNotional;
	double tradeRate;
	int frequency; // use 1 for annual, 2 for semi-annual etc

	double getAnnuity() const
	{
		// implement this where assuming zero rate is 4% per annum for discouting.

		return 0;
	};
};