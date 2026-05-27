#ifndef _TREE_PRODUCT_H
#define _TREE_PRODUCT_H
#include "Date.h"
#include "Trade.h"
#include "Types.h"

// this class provide a common member function interface for option type of trade.
class Option : public Trade
{
public:
	Option(const Date& tradeDate, OptionType optionType) : Trade("Option", tradeDate) {
		optType = optionType;
	};
	virtual const Date& GetExpiry() const = 0;
	virtual double ValueAtNode(double stockPrice, double t, double continuationValue) const = 0;

	OptionType optType;
};

#endif
