#ifndef MARKET_H
#define MARKET_H

#include "core/Date.h"
#include "instruments/Types.h"
#include <map>
#include <stdexcept>
#include <unordered_map>

class RateCurve {
public:
  RateCurve() = default;

  explicit RateCurve(std::string _name) : name{std::move(_name)} {};

  void addRate(Date tenor, double rate);

  double getRate(Date tenor) const;

  void bump(double bumpValue) {
    for (auto &[date, rate] : curveData) {
      rate += bumpValue;
    }
  }

  void display() const;

private:
  std::string name{};
  std::map<Date, double> curveData{};
};

class VolCurve {
public:
  VolCurve() = default;

  explicit VolCurve(std::string _name) : name{std::move(_name)} {};

  void addVol(Date tenor, double vol);

  double getVol(Date tenor) const;

  void bump(double bumpValue) {
    for (auto &[date, vol] : volData) {
      vol += bumpValue;
    }
  }

  void display() const;

private:
  std::string name{};
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

  Date getCurrentDate() const { return m_asOf; }
  void setCurrentDate(Date now) { m_asOf = now; }

  const RateCurve &getRateCurve(Currency ccy) const {
    switch (ccy) {
      using enum Currency;

    case USD:
      return getMarketData<RateCurve>("USD-SOFR");
    default:
      throw std::invalid_argument(
          "Error: No rate curve mapped for this currency");
    }
  }

  void bumpRateCurve(Currency ccy, double bumpValue);
  void bumpVolCurve(const std::string &name, double bumpValue);

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
