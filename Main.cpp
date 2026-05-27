#include <fstream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <memory>

#include "Market.h"
#include "Pricer.h"
#include "EuropeanTrade.h"
#include "Bond.h"
#include "Swap.h"
#include "AmericanTrade.h"


std::string getCurrentSystemTime()
{
	auto now = std::chrono::system_clock::now();
	time_t now_c = std::chrono::system_clock::to_time_t(now);
	tm now_tm;
	//localtime_r(&now_c, &now_tm);
	std::stringstream ss;
	ss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
	return ss.str();
}

void readFromFile(const std::string& fileName, std::string& outPut)
{
	std::string lineText;
	std::ifstream MyReadFile(fileName);
	while (std::getline(MyReadFile, lineText))
	{
		outPut.append(lineText);
	}
	MyReadFile.close();
}

int main()
{
	// task 1, create an market data object, and update the market data from from txt file
	Date valueDate(2023, 12, 31);
	time_t t = time(nullptr);
	struct tm timeInfo;

	std::cout << valueDate << std::endl;
	Market* mkt = new Market(valueDate);
	/*
	load data from file and update market object with data
	*/

	RateCurve rc = RateCurve("USD-SOFR");
	Date t_on = Date(2025, 5, 15);
	Date t_tn = Date(2025, 5, 16);
	Date t_1w = Date(2025, 5, 22);

	rc.addRate(t_on, 0.032);
	rc.addRate(t_tn, 0.032);
	rc.addRate(t_1w, 0.031);
	rc.display();
	mkt->addCurve("USD-SOFR", rc);
	mkt->Print();

	// task 2, create a portfolio of bond, swap, european option, american option
	// for each time, at least should have long / short, different tenor or expiry, different underlying
	// totally no less than 16 trades
	std::vector<Trade*> myPortfolio;
	Date tradeDate = valueDate;
	Date startDate = Date(2023, 12, 31);
	Date expiryDate = Date(2025, 12, 31);
	Trade* swapTrade = new Swap(tradeDate, startDate, expiryDate, 1000000, 0.05, 2);
	Trade* bondTrade = new Bond(tradeDate, "testBond1", startDate, expiryDate, 100000, 0.025, 1);
	Trade* eCallOption = new EuropeanOption(tradeDate, 10000, 100, expiryDate, OptionType::Call);
	Trade* aOption = new AmericanOption(tradeDate, 10000,  100, expiryDate, OptionType::Call);

	myPortfolio.push_back(swapTrade);
	myPortfolio.push_back(bondTrade);
	myPortfolio.push_back(eCallOption);
	myPortfolio.push_back(aOption);

	// task 3, creat a pricer and price the portfolio, output the pricing result of each deal.
	auto pricer = CRRBinomialTreePricer(20);
	for (auto& trade : myPortfolio)
	{
		double pv = pricer.Price(*mkt, trade);
		// log pv details out in a file
	}

	// task 4, analyzing pricing result
	//  a) compare CRR binomial tree result for an european option vs Black model
	//  b) compare CRR binomial tree result for an american option call vs european option call, and put

	// final
	for (auto& trade : myPortfolio)
	{
		delete trade;
	}

	delete mkt;

	std::cout << "Project build successfully!" << std::endl;
	return 0;
}
