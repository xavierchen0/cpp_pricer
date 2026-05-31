#ifndef MARKET_H
#define MARKET_H

#include "Date.h"
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

class RateCurve {
public:
  RateCurve() = default;

  // Use of 'explicit' keyword to prevent implicit conversions, thus leading to
  // unwanted bugs.
  //
  // Use of std::move to convert the parameter _name from an lvalue to rvalue to
  // invoke std::string's move constructor, thereby eliminating unnecessary
  // copying to create a temporary object.
  explicit RateCurve(std::string _name) : name{std::move(_name)} {};

  // Use of reference & prevents copying of Date object to initialise the tenor
  // parameter, thereby saving one copy operation.
  void addRate(const Date &tenor, double rate);

  double getRate(const Date &tenor) const;

  void display() const;

private:
  std::string name;
  // std::map is a sorted associative container that stores unique key-value
  // pairs ordered by their keys.
  std::map<Date, double> curveData{};
};

class VolCurve {
public:
  VolCurve() = default;

  // Use of 'explicit' keyword to prevent implicit conversions, thus leading to
  // unwanted bugs.
  //
  // Use of std::move to convert the parameter _name from an lvalue to rvalue to
  // invoke std::string's move constructor, thereby eliminating unnecessary
  // copying to create a temporary object.
  explicit VolCurve(std::string _name) : name{std::move(_name)} {};

  // Use of reference & prevents copying of Date object to initialise the tenor
  // parameter, thereby saving one copy operation.
  void addVol(const Date &tenor, double vol);

  double getVol(const Date &tenor) const;

  void display() const;

private:
  std::string name;
  // std::map is a sorted associative container that stores unique key-value
  // pairs ordered by their keys.
  std::map<Date, double> volData{};
};

class Market {
public:
  Date asOf;
  std::string name;

  Market() {
    std::cout << "default market constructor is called by object@" << this
              << std::endl;
  }

  Market(const Date &now) : asOf(now) {
    std::cout << "market constructor is called by object@" << this << std::endl;
    name = "test_market";
  }

  Market(const Market &other) : asOf(other.asOf) {
    std::cout << "copy constructor is called by object@" << this << std::endl;
    // deep copy behaviour
    asOf = other.asOf;
    name = other.name;
  }

  Market &operator=(const Market &other) {
    std::cout << "assignment constructor is called by object@" << this
              << std::endl;
    if (this != &other) { // Check for self-assignment
      asOf = other.asOf;
      name = other.name;
    }
    return *this;
  }

  ~Market() { std::cout << "Market destructor is called" << std::endl; }

  void Print() const;
  void addCurve(const std::string &curveName,
                const RateCurve &curve); // implement this
  void addVolCurve(const std::string &curveName,
                   const VolCurve &curve); // implement this
  void addBondPrice(const std::string &bondName, double price); // implement
                                                                // this
  void addVolCurve(const std::string &stockName, double price); // implement
                                                                // this

  inline RateCurve getCurve(const std::string &name) { return curves[name]; };
  inline VolCurve getVolCurve(const std::string &name) { return vols[name]; };

private:
  std::unordered_map<std::string, VolCurve> vols;
  std::unordered_map<std::string, RateCurve> curves;
  std::unordered_map<std::string, double> bondPrices;
  std::unordered_map<std::string, double> stockPrices;
};

std::ostream &operator<<(std::ostream &os, const Market &obj);
std::istream &operator>>(std::istream &is, Market &obj);

#endif
