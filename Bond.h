#pragma once
#include "Trade.h"
#include "Market.h"

class Bond : public Trade
{

public:
	Bond(Date start, Date end) : Trade("BondTrade", start) 
	{	
	};

	Bond(const Date &tradeDate, const std::string& name, const Date &start, const Date &end,
		 double notional, int couponFreq, double price) : Trade("BondTrade", tradeDate)
	{
		startDate = start;
		endDate = end;
		bondName = name;
		bondNotional = notional;
		frequecy = couponFreq;
		tradePrice = price;
	};

	inline double Payoff(double marketPrice) const
	{
		return 0;
	}; // implement this

	void setFreq(int freq) {
		frequecy = freq;
	}

private:
	std::string bondName;
	double bondNotional;
	double tradePrice;
	int frequecy;
	Date startDate;
	Date endDate;
};
