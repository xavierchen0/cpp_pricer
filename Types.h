#ifndef TYPES_H
#define TYPES_H

#include <ostream>
#include <string_view>

enum class TradeType {
  Bond,
};

enum OptionType { Call, Put, BinaryCall, BinaryPut };

inline std::string_view getTradeTypeName(TradeType tradeType) {

  switch (tradeType) {
    using enum TradeType;

  case Bond:
    return "Bond";
  default:
    return "???";
  }
}

inline std::ostream &operator<<(std::ostream &os, TradeType tradeType) {
  os << getTradeTypeName(tradeType);
  return os;
}

#endif
