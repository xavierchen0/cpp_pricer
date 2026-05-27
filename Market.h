#ifndef MARKET_H
#define MARKET_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include "Date.h"



class RateCurve {
public:
	RateCurve() {};
	RateCurve(const std::string& _name) : name(_name) {};
	void addRate(Date tenor, double rate);
	double getRate(Date tenor) const; //implement this function using linear interpolation
	void display() const;

private:
	std::string name;
	std::vector<Date> tenorDates;
	std::vector<double> rates;
};

class VolCurve { // atm vol curve without smile
public:
	VolCurve() {}
	VolCurve(const std::string& _name) : name(_name) {};
	void addVol(Date tenor, double rate); //implement this
	double getVol(Date tenor) const; //implement this function using linear interpolation
	void display() const; //implement this

private:
	std::string name;
	std::vector<Date> tenors;
	std::vector<double> vols;
};

class Market
{
public:
	Date asOf;
	std::string name;

	Market() {
		std::cout << "default market constructor is called by object@" << this << std::endl;
	}

	Market(const Date& now) : asOf(now) {
		std::cout << "market constructor is called by object@" << this << std::endl;
		name = "test_market";
	}

	Market(const Market& other) : asOf(other.asOf) {
		std::cout << "copy constructor is called by object@" << this << std::endl;
		// deep copy behaviour
		asOf = other.asOf;
		name = other.name;
	}

	Market& operator=(const Market& other) {
		std::cout << "assignment constructor is called by object@" << this << std::endl;
		if (this != &other) {  // Check for self-assignment
			asOf = other.asOf;
			name = other.name;
		}
		return *this;
	}

	~Market() {
		std::cout << "Market destructor is called" << std::endl;
	}

	void Print() const;
	void addCurve(const std::string& curveName, const RateCurve& curve);//implement this
	void addVolCurve(const std::string& curveName, const VolCurve& curve);//implement this
	void addBondPrice(const std::string& bondName, double price);//implement this
	void addVolCurve(const std::string& stockName, double price);//implement this

	inline RateCurve getCurve(const std::string& name) { return curves[name]; };
	inline VolCurve getVolCurve(const std::string& name) { return vols[name]; };

private:

	std::unordered_map<std::string, VolCurve> vols;
	std::unordered_map<std::string, RateCurve> curves;
	std::unordered_map<std::string, double> bondPrices;
	std::unordered_map<std::string, double> stockPrices;
};

std::ostream& operator<<(std::ostream& os, const Market& obj);
std::istream& operator>>(std::istream& is, Market& obj);


#endif
