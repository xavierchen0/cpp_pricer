#include "market/Market.h"
#include "core/Date.h"
#include <iomanip>
#include <iostream>

void RateCurve::addRate(Date tenor, double rate) {
  // If tenor does not exist in curveData, add to curveData,
  // otherwise, overwrite rate.
  curveData[tenor] = rate / 100.0;
}

double RateCurve::getRate(Date tenor) const {
  // Throw runtime error if rate cuve is empty; Unable to proceed with pricing;
  // Might be due to forgetting to load the market rate curve data.
  if (curveData.empty()) {
    throw std::runtime_error("Error: RateCurve '" + name + "' is empty.");
  }

  // Initialise std::map's iterator object pointing to the first key that is
  // greater than or equal to tenor.
  const auto it{curveData.lower_bound(tenor)};

  // Case 1: tenor <= smallest date in std::map
  if (it == curveData.begin()) {
    return it->second;
  }

  // Case 2: tenor > largest date in std::map
  if (it == curveData.end()) {
    // Move iterator back by one position to access the value of the largest
    // date in std::map.
    //
    // std::map::end returns an iterator pointing to the theoretical element
    // just after the last element in a std::map
    return std::prev(it)->second;
  }

  // Case 3: tenor is between valid dates in std::map
  auto it_prev{std::prev(it)};
  const Date &t1{it_prev->first};
  double r1{it_prev->second};

  const Date &t2{it->first};
  double r2{it->second};

  double t21{t2 - t1};
  return r1 + ((tenor - t1) / t21 * (r2 - r1));
}

void RateCurve::display() const {
  std::cout << "\n----------------------------------------\n";
  std::cout << "  RATE CURVE: " << name << "\n";
  std::cout << "----------------------------------------\n";
  std::cout << std::left << std::setw(15) << "  Tenor (Date)" << " | "
            << "Rate (%)\n";
  std::cout << "----------------------------------------\n";

  for (const auto &[date, rate] : curveData) {
    std::cout << "  " << std::left << std::setw(13) << date << " | "
              << std::fixed << std::setprecision(4) << (rate * 100.0) << "%\n";
  }

  std::cout << "----------------------------------------\n";
}

void VolCurve::addVol(Date tenor, double vol) {
  // If tenor does not exist in volData, add to volData,
  // otherwise, overwrite rate.
  volData[tenor] = vol / 100.0;
}

double VolCurve::getVol(Date tenor) const {
  // Throw runtime error if vol cuve is empty; Unable to proceed with pricing;
  // Might be due to forgetting to load the market vol curve data.
  if (volData.empty()) {
    throw std::runtime_error("Error: VolCurve '" + name + "' is empty.");
  }

  // Initialise std::map's iterator object pointing to the first key that is
  // greater than or equal to tenor.
  const auto it{volData.lower_bound(tenor)};

  // Case 1: tenor <= smallest date in std::map
  if (it == volData.begin()) {
    return it->second;
  }

  // Case 2: tenor > largest date in std::map
  if (it == volData.end()) {
    // Move iterator back by one position to access the value of the largest
    // date in std::map.
    //
    // std::map::end returns an iterator pointing to the theoretical element
    // just after the last element in a std::map
    return std::prev(it)->second;
  }

  // Case 3: tenor is between valid dates in std::map
  auto it_prev{std::prev(it)};
  const Date &t1{it_prev->first};
  double r1{it_prev->second};

  const Date &t2{it->first};
  double r2{it->second};

  double t21{t2 - t1};
  return r1 + ((tenor - t1) / t21 * (r2 - r1));
}

void VolCurve::display() const {
  std::cout << "\n----------------------------------------\n";
  std::cout << "  VOLATILITY CURVE: " << name << "\n";
  std::cout << "----------------------------------------\n";
  std::cout << std::left << std::setw(15) << "  Tenor (Date)" << " | "
            << "Volatility (%)\n";
  std::cout << "----------------------------------------\n";

  for (const auto &[date, vol] : volData) {
    std::cout << "  " << std::left << std::setw(13) << date << " | "
              << std::fixed << std::setprecision(2) << (vol * 100.0) << "%\n";
  }

  std::cout << "----------------------------------------\n";
}

void Market::display() const {
  std::cout << "\n==================================================\n";
  std::cout << " MARKET DATA  [Valuation Date: " << m_asOf << "]\n";
  std::cout << "==================================================\n";

  for (const auto &[name, rateCurve] : m_rates) {
    rateCurve.display();
  }

  for (const auto &[name, volCurve] : m_vols) {
    volCurve.display();
  }

  std::cout << "\n  BOND PRICES:\n";
  std::cout << "  ----------------------------------\n";
  for (const auto &[name, price] : m_bondPrices) {
    std::cout << "    " << std::left << std::setw(15) << name << " => "
              << std::fixed << std::setprecision(2) << price.value << "\n";
  }
  std::cout << "  ----------------------------------\n";

  std::cout << "\n  STOCK PRICES:\n";
  std::cout << "  ----------------------------------\n";
  for (const auto &[name, price] : m_stockPrices) {
    std::cout << "    " << std::left << std::setw(15) << name << " => "
              << std::fixed << std::setprecision(2) << price.value << "\n";
  }
  std::cout << "  ----------------------------------\n";

  std::cout << "\n==================================================\n";
}

void Market::bumpRateCurve(Currency ccy, double bumpValue) {
  switch (ccy) {
    using enum Currency;

  case USD:
    getMap<RateCurve>().at("USD-SOFR").bump(bumpValue);
    break;
  default:
    throw std::invalid_argument(
        "Error: No rate curve mapped for this currency");
  }
}

void Market::bumpVolCurve(const std::string &name, double bumpValue) {
  getMap<VolCurve>().at(name).bump(bumpValue);
}
