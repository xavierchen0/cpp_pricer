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
  NA,
};

enum class OptionExerciseStyle {
  European,
  American,
};

enum class OptionPayoff {
  Vanilla,
  Binary,
  CallSpread,
  NA,
};

enum class Currency {
  USD,
  SGD,
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

inline std::string_view getName(OptionExerciseStyle optionExerciseStyle) {

  switch (optionExerciseStyle) {
    using enum OptionExerciseStyle;

  case European:
    return "European";
  case American:
    return "American";
  default:
    return "???";
  }
}

inline std::string_view getName(OptionPayoff optionPayoff) {

  switch (optionPayoff) {
    using enum OptionPayoff;

  case Vanilla:
    return "Vanilla";
  case Binary:
    return "Binary";
  case CallSpread:
    return "CallSpread";
  default:
    return "???";
  }
}

inline std::string_view getName(Currency tradeCcy) {

  switch (tradeCcy) {
    using enum Currency;

  case USD:
    return "USD";
  case SGD:
    return "SGD";
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

inline std::ostream &operator<<(std::ostream &os,
                                OptionExerciseStyle optionExerciseStyle) {
  os << getName(optionExerciseStyle);
  return os;
}

inline std::ostream &operator<<(std::ostream &os, OptionPayoff optionPayoff) {
  os << getName(optionPayoff);
  return os;
}

inline std::ostream &operator<<(std::ostream &os, Currency tradeCcy) {
  os << getName(tradeCcy);
  return os;
}

#endif
