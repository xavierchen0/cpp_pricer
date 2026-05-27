#pragma once

using namespace std;
// Abstract creator class
class TradeFactory
{
public:
	virtual shared_ptr<Trade> createTrade(const string &type, const Date &tradeDate, const Date &expiryDate) = 0;
	virtual ~TradeFactory()
	{
		cout << "trade factor is destroyed" << endl;
	}
};

// Concrete creator class - create option type of trades
class LinearTradeFactory : public TradeFactory
{
public:
	shared_ptr<Trade> createTrade(const string& type, const Date& tradeDate, const Date& expiryDate) override
	{
		if (type == "swap")
		{
			return make_shared<Swap>(tradeDate, expiryDate);
		}
		else if (type == "bond")
		{
			return make_shared<Bond>(tradeDate, expiryDate);
		}
		else
			return nullptr;
	}
};

// Concrete creator class - create option type of trades
class OptionTradeFactory : public TradeFactory
{
public:
	shared_ptr<Trade> createTrade(const string &type, const Date &tradeDate, const Date &expiryDate) override
	{
		if (type == "european")
		{
			return make_shared<EuropeanOption>(tradeDate, expiryDate);
		}
		else if (type == "american")
		{
			return make_shared<AmericanOption>(tradeDate, expiryDate);
		}
		else
			return nullptr;
	}
};