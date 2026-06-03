#ifndef MARKET_H
#define MARKET_H

#include "core/Date.h"
#include <map>
#include <unordered_map>

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

  // For a small Date object, const ref is slower for the
  // CPU because of the additional step to dereference the object first before
  // fetching the data. In other words, the CPU has to read and look up the
  // memory address (dereference) before it can fetch the object, instead of
  // directly fetching the object.
  void addRate(Date tenor, double rate);

  double getRate(Date tenor) const;

  void display() const;

private:
  std::string name{};
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

  void addVol(Date tenor, double vol);

  double getVol(Date tenor) const;

  void display() const;

private:
  std::string name{};
  // std::map is a sorted associative container that stores unique key-value
  // pairs ordered by their keys.
  std::map<Date, double> volData{};
};

struct BondPrice {
  double value{};
  operator double() const { return value; }
};

struct StockPrice {
  double value{};
  operator double() const { return value; }
};

class Market {
public:
  Market() = default;

  Market(Date now) : m_asOf(now) {}

  Date getCurrentDate() const { return m_asOf; }

  // For a small Date object, const ref is slower for the
  // CPU because of the additional step to dereference the object first before
  // fetching the data. In other words, the CPU has to read and look up the
  // memory address (dereference) before it can fetch the object, instead of
  // directly fetching the object.
  void setCurrentDate(Date now) { m_asOf = now; }

  // Setter to add/overwrite market data in their respective containers
  template <typename T> void addMarketData(std::string name, T data) {
    getMap<T>().insert_or_assign(std::move(name), std::move(data));
  }

  // Getter to get market data in their respective containers
  template <typename T> const T &getMarketData(const std::string &name) const {
    return getMap<T>().at(name);
  }

  void display() const;

private:
  Date m_asOf{};

  std::unordered_map<std::string, RateCurve> m_rates{};
  std::unordered_map<std::string, VolCurve> m_vols{};
  std::unordered_map<std::string, BondPrice> m_bondPrices{};
  std::unordered_map<std::string, StockPrice> m_stockPrices{};

  template <typename T> auto &getMap();             // for setter
  template <typename T> const auto &getMap() const; // for getter
};

template <> inline auto &Market::getMap<RateCurve>() { return m_rates; }
template <> inline auto &Market::getMap<VolCurve>() { return m_vols; }
template <> inline auto &Market::getMap<BondPrice>() { return m_bondPrices; }
template <> inline auto &Market::getMap<StockPrice>() { return m_stockPrices; }

template <> inline const auto &Market::getMap<RateCurve>() const {
  return m_rates;
}
template <> inline const auto &Market::getMap<VolCurve>() const {
  return m_vols;
}
template <> inline const auto &Market::getMap<BondPrice>() const {
  return m_bondPrices;
}
template <> inline const auto &Market::getMap<StockPrice>() const {
  return m_stockPrices;
}

#endif
