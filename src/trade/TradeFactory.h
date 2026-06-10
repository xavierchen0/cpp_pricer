#ifndef TRADE_FACTORY_H
#define TRADE_FACTORY_H

#include "engine/Payoff.h"
#include "engine/Pricer.h"
#include "instruments/Bond.h"
#include "instruments/Option.h"
#include "instruments/Swap.h"
#include "instruments/Trade.h"
#include "instruments/Types.h"
#include "trade/TradeRecord.h"
#include <memory>
#include <stdexcept>

class TradeFactory {
public:
  static std::unique_ptr<ITrade> createTrade(const TradeRecord &tradeRecord) {
    if (tradeRecord.tradeType == TradeType::Bond) {
      return std::make_unique<Bond>(
          tradeRecord.tradeDate, tradeRecord.startDate, tradeRecord.endDate,
          tradeRecord.tradeCcy, tradeRecord.name, tradeRecord.notional,
          tradeRecord.rate, tradeRecord.frequency);
    } else if (tradeRecord.tradeType == TradeType::Swap) {
      return std::make_unique<Swap>(tradeRecord.tradeDate,
                                    tradeRecord.startDate, tradeRecord.endDate,
                                    tradeRecord.tradeCcy, tradeRecord.notional,
                                    tradeRecord.rate, tradeRecord.frequency);
    } else if (tradeRecord.tradeType == TradeType::Option) {
      std::unique_ptr<IOptionPayoff> payoff{};

      switch (tradeRecord.optionPayoff) {
      case OptionPayoff::Vanilla:
        payoff = std::make_unique<VanillaOptionPayoff>(tradeRecord.optionRight,
                                                       tradeRecord.strike);
        break;
      case OptionPayoff::Binary:
        payoff = std::make_unique<BinaryOptionPayoff>(tradeRecord.optionRight,
                                                      tradeRecord.strike);
        break;
      default:
        break;
      }

      // Make default pricer to be the Black-Scholes pricer
      auto pricer{std::make_unique<BlackScholesOptionPricer>()};

      return std::make_unique<Option>(
          tradeRecord.tradeDate, tradeRecord.endDate, tradeRecord.tradeCcy,
          tradeRecord.name, tradeRecord.optionRight,
          tradeRecord.optionExerciseStyle, std::move(payoff), std::move(pricer),
          tradeRecord.strike);
    } else {
      throw std::invalid_argument(std::format("Error: Invalid Trade Type"));
    }
  }
};

#endif
