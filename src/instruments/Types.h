#ifndef TYPES_H
#define TYPES_H

#include <ostream>
#include <string_view>

enum class TradeType {
  Bond,
  Swap,
  Option,
};

enum class OptionRight {
  Call,
  Put,
};

enum class Currency {
  USD,
};

inline std::string_view getName(TradeType tradeType) {

  switch (tradeType) {
    using enum TradeType;

  case Bond:
    return "Bond";
  case Swap:
    return "Swap";
  case Option:
    return "Option";
  default:
    return "???";
  }
}

inline std::string_view getName(OptionRight optionRight) {

  switch (optionRight) {
    using enum OptionRight;

  case Call:
    return "Call";
  case Put:
    return "Put";
  default:
    return "???";
  }
}

inline std::string_view getName(Currency tradeCcy) {

  switch (tradeCcy) {
    using enum Currency;

  case USD:
    return "USD";
  default:
    return "???";
  }
}

inline std::ostream &operator<<(std::ostream &os, TradeType tradeType) {
  os << getName(tradeType);
  return os;
}

inline std::ostream &operator<<(std::ostream &os, OptionRight optionRight) {
  os << getName(optionRight);
  return os;
}

inline std::ostream &operator<<(std::ostream &os, Currency tradeCcy) {
  os << getName(tradeCcy);
  return os;
}

#endif
