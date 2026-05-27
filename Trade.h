#pragma once
#include<string>
#include "Date.h"
#include "Market.h"


class Trade {
public:
	Trade() {};
	Trade(const std::string& _type, const Date& _tradeDate)
		: tradeType(_type), tradeDate(_tradeDate) {
	};
	inline std::string getType() { return tradeType; };
	virtual double Payoff(double marketPrice) const = 0; //pure virtual function, need to be implemented by derived class
	virtual ~Trade() {};

protected:
	std::string tradeType;
	Date tradeDate;
};