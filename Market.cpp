#include "Market.h"
#include <stdexcept>

void RateCurve::addRate(const Date &tenor, double rate) {
  // If tenor does not exist in curveData, add to curveData,
  // otherwise, overwrite rate.
  curveData[tenor] = rate;
}

double RateCurve::getRate(const Date &tenor) const {
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
  std::cout << "rate curve:" << name << '\n';
  for (const auto &[date, rate] : curveData) {
    std::cout << date << ":" << rate << '\n';
  }
}

void VolCurve::addVol(const Date &tenor, double vol) {
  // If tenor does not exist in volData, add to volData,
  // otherwise, overwrite rate.
  volData[tenor] = vol;
}

double VolCurve::getVol(const Date &tenor) const {
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
  std::cout << "vol curve:" << name << '\n';
  for (const auto &[date, vol] : volData) {
    std::cout << date << ":" << vol << '\n';
  }
}

void Market::Print() const {
  std::cout << "market asof: " << asOf << std::endl;

  for (auto curve : curves) {
    curve.second.display();
  }
  for (auto vol : vols) {
    vol.second.display();
  }
  /*
  add display for bond price and stock price

  */
}

void Market::addCurve(const std::string &curveName, const RateCurve &curve) {
  curves.emplace(curveName, curve);
}

std::ostream &operator<<(std::ostream &os, const Market &mkt) {
  os << mkt.asOf << std::endl;
  return os;
}

std::istream &operator>>(std::istream &is, Market &mkt) {
  is >> mkt.asOf;
  return is;
}
