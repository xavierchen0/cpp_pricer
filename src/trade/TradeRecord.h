#ifndef TRADE_RECORD_H
#define TRADE_RECORD_H

#include "core/Date.h"
#include "instruments/Types.h"

// Struct to hold fields of each row in trade file
struct TradeRecord {
  int id{};
  TradeType tradeType{};
  Currency tradeCcy{};
  Date tradeDate{};
  Date startDate{};
  Date endDate{};
  double notional{};
  std::string name{};
  double rate{};
  double strike{};
  double frequency{};
  OptionRight optionRight{};
  OptionExerciseStyle optionExerciseStyle{};
  OptionPayoff optionPayoff{};
};

#endif
