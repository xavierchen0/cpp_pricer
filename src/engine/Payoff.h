#ifndef PAYOFF_H
#define PAYOFF_H

#include "instruments/Types.h"
#include <algorithm>
#include <stdexcept>

// Strategy Design Pattern
// Abstract Base Class for different Option payoff
class IOptionPayoff {
public:
  virtual ~IOptionPayoff() = default;

  virtual double operator()(double underlyingSpotPrice) const = 0;
};

class VanillaOptionPayoff final : public IOptionPayoff {
public:
  VanillaOptionPayoff(OptionRight optionRight, double strike)
      : m_optionRight{optionRight}, m_strike{strike} {}

  double operator()(double underlyingSpotPrice) const override {
    switch (m_optionRight) {
      using enum OptionRight;

    case Call:
      return std::max(0.0, underlyingSpotPrice - m_strike);
    case Put:
      return std::max(0.0, m_strike - underlyingSpotPrice);
    default:
      throw std::invalid_argument("Error: Unknown OptionRight enumerator");
    }
  }

private:
  OptionRight m_optionRight{};
  double m_strike{};
};

class BinaryOptionPayoff final : public IOptionPayoff {
public:
  BinaryOptionPayoff(OptionRight optionRight, double strike)
      : m_optionRight{optionRight}, m_strike{strike} {}

  double operator()(double underlyingSpotPrice) const override {
    switch (m_optionRight) {
      using enum OptionRight;

    case Call:
      return underlyingSpotPrice >= m_strike ? 1.0 : 0.0;
    case Put:
      return underlyingSpotPrice <= m_strike ? 1.0 : 0.0;
    default:
      throw std::invalid_argument("Error: Unknown OptionRight enumerator");
    }
  }

private:
  OptionRight m_optionRight{};
  double m_strike{};
};

#endif
