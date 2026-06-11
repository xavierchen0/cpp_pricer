#include "market/Loaders.h"
#include "instruments/Types.h"
#include "market/Market.h"
#include "trade/TradeFactory.h"
#include "trade/TradeRecord.h"
#include <algorithm>
#include <charconv>
#include <format>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace {
// Helper function to convert tenors (e.g. 3M, 6M) to a Date object relative to
// now parameter
Date parseTenor(const Date &now, const std::string_view tenor) {
  if (tenor.empty()) {
    throw std::runtime_error("Error: Empty tenor string found.");
  }

  if (tenor == "ON") {
    return now + (1.0 / 360.0);
  }

  char unit{tenor.back()};

  std::string_view timeStr{tenor.substr(0, tenor.size() - 1)};
  double time{};
  auto [ptr, ec]{
      std::from_chars(timeStr.data(), timeStr.data() + timeStr.size(), time)};

  if (ec != std::errc{} || ptr != (timeStr.data() + timeStr.size())) {
    throw std::invalid_argument(
        std::format("Error: Invalid numeric prefix in tenor: {}", tenor));
  }

  constexpr double daysInYear{360.0};
  constexpr double daysInWeek{7.0};
  constexpr double monthsInYear{12.0};

  if (unit == 'D' || unit == 'd') {
    return now + (time / daysInYear);
  } else if (unit == 'W' || unit == 'w') {
    return now + ((time * daysInWeek) / daysInYear);
  } else if (unit == 'M' || unit == 'm') {
    return now + (time / monthsInYear);
  } else if (unit == 'Y' || unit == 'y') {
    return now + time;
  }

  throw std::invalid_argument(std::format("Error: Unknown tenor: {}", tenor));
}

// Helper function to parse dates
Date parseDate(const std::string_view dataStr) {
  if (dataStr.size() != 10 || dataStr[4] != '-' || dataStr[7] != '-') {
    throw std::invalid_argument(std::format(
        "Error: Invalid date format. Expected YYYY-MM-DD: {}", dataStr));
  }

  int year{};
  int month{};
  int day{};
  std::from_chars(dataStr.data(), dataStr.data() + 4, year);
  std::from_chars(dataStr.data() + 5, dataStr.data() + 7, month);
  std::from_chars(dataStr.data() + 8, dataStr.data() + 10, day);

  return Date{year, month, day};
}

// Helper function to parse values with percentage sign
double parsePercentage(const std::string_view pctValue) {
  if (pctValue.empty()) {
    throw std::runtime_error("Error: Empty percentage value found.");
  }

  if (pctValue.back() != '%') {
    throw std::invalid_argument(std::format(
        "Error: Invalid percentage format (missing '%'): {}", pctValue));
  }

  std::string_view valueStr{pctValue.substr(0, pctValue.size() - 1)};

  if (valueStr.empty()) {
    throw std::invalid_argument(
        std::format("Error: No numeric value before '%': {}", pctValue));
  }

  double value{};
  auto [ptr, ec]{std::from_chars(valueStr.data(),
                                 valueStr.data() + valueStr.size(), value)};

  if (ec != std::errc{} || ptr != (valueStr.data() + valueStr.size())) {
    throw std::invalid_argument(
        std::format("Invalid numeric prefix in valueStr: {}", pctValue));
  }

  return value;
}

// Helper function to trim leading and trailing whitespaces away in string
std::string_view trimView(const std::string_view str) {
  constexpr std::string_view whitespace{" \t\r\n"};

  const auto first{str.find_first_not_of(whitespace)};

  if (first == std::string_view::npos) { // str only contains whitespace
    return {};
  }

  const auto last{str.find_last_not_of(whitespace)};

  return str.substr(first, last - first + 1);
}

// Helper function to parse delimited strings using string_view without making
// copies
std::vector<std::string_view> splitView(const std::string_view str,
                                        char delimiter) {
  std::vector<std::string_view> values{};
  size_t start{0};
  size_t end{str.find(delimiter)};

  while (end != std::string_view::npos) {
    values.push_back(str.substr(start, end - start));
    start = end + 1;
    end = str.find(delimiter, start);
  }
  values.push_back(str.substr(start));
  return values;
}
} // namespace

void loadRateCurve(const std::filesystem::path &filePath, Market &market) {

  std::ifstream file{filePath};
  if (!file.is_open()) {
    throw std::runtime_error(std::format(
        "Error: Could not open rate curve file: {}", filePath.string()));
  }

  std::string curveName{};
  if (!std::getline(file, curveName)) {
    throw std::runtime_error(
        std::format("Error: Empty curve file: {}", filePath.string()));
  }

  curveName = std::string(trimView(curveName));
  if (curveName.empty()) {
    throw std::runtime_error(std::format(
        "Error: Empty curve name after trimming: {}", filePath.string()));
  }

  RateCurve rc{curveName};
  std::string line{};
  while (std::getline(file, line)) {
    std::string_view lineView{trimView(line)};
    if (lineView.empty()) {
      continue;
    }

    size_t colonPos{lineView.find(':')};
    if (colonPos == std::string::npos) {
      continue;
    }

    std::string_view tenorStr{trimView(lineView.substr(0, colonPos))};
    std::string_view rateStr{trimView(lineView.substr(colonPos + 1))};

    if (tenorStr.empty() || rateStr.empty()) {
      continue;
    }

    Date tenorDate{parseTenor(market.getCurrentDate(), tenorStr)};
    double rateVal{parsePercentage(rateStr)};

    rc.addRate(tenorDate, rateVal);
  }
  market.addMarketData(curveName, rc);
}

void loadVolCurve(const std::filesystem::path &filePath,
                  const std::string &curveName, Market &market) {

  std::ifstream file{filePath};
  if (!file.is_open()) {
    throw std::runtime_error(std::format(
        "Error: Could not open vol curve file: {}", filePath.string()));
  }

  VolCurve vc{curveName};
  std::string line{};
  while (std::getline(file, line)) {
    std::string_view lineView{trimView(line)};
    if (lineView.empty()) {
      continue;
    }

    size_t colonPos{lineView.find(':')};
    if (colonPos == std::string::npos) {
      continue;
    }

    std::string_view tenorStr{trimView(lineView.substr(0, colonPos))};
    std::string_view volStr{trimView(lineView.substr(colonPos + 1))};

    if (tenorStr.empty() || volStr.empty()) {
      continue;
    }

    Date tenorDate{parseTenor(market.getCurrentDate(), tenorStr)};
    double volVal{parsePercentage(volStr)};

    vc.addVol(tenorDate, volVal);
  }
  market.addMarketData(curveName, vc);
}

void loadStockPrices(const std::filesystem::path &filePath, Market &market) {

  std::ifstream file{filePath};
  if (!file.is_open()) {
    throw std::runtime_error(std::format(
        "Error: Could not open stock price file: {}", filePath.string()));
  }

  std::string line{};
  while (std::getline(file, line)) {
    std::string_view lineView{trimView(line)};
    if (lineView.empty()) {
      continue;
    }

    size_t colonPos{lineView.find(':')};
    if (colonPos == std::string::npos) {
      continue;
    }

    std::string_view stockStr{trimView(lineView.substr(0, colonPos))};
    std::string_view priceStr{trimView(lineView.substr(colonPos + 1))};

    if (stockStr.empty() || priceStr.empty()) {
      continue;
    }

    double priceVal{};
    auto [ptr, ec]{std::from_chars(
        priceStr.data(), priceStr.data() + priceStr.size(), priceVal)};

    if (ec != std::errc{} || ptr != (priceStr.data() + priceStr.size())) {
      throw std::invalid_argument(
          std::format("Error: Invalid stock price: {}", std::string(priceStr)));
    }
    market.addMarketData(std::string(stockStr), StockPrice{priceVal});
  }
}

void loadBondPrices(const std::filesystem::path &filePath, Market &market) {

  std::ifstream file{filePath};
  if (!file.is_open()) {
    throw std::runtime_error(std::format(
        "Error: Could not open bond price file: {}", filePath.string()));
  }

  std::string line{};
  while (std::getline(file, line)) {
    std::string_view lineView{trimView(line)};
    if (lineView.empty()) {
      continue;
    }

    size_t colonPos{lineView.find(':')};
    if (colonPos == std::string::npos) {
      continue;
    }

    std::string_view bondStr{trimView(lineView.substr(0, colonPos))};
    std::string_view priceStr{trimView(lineView.substr(colonPos + 1))};

    if (bondStr.empty() || priceStr.empty()) {
      continue;
    }

    double priceVal{};
    auto [ptr, ec]{std::from_chars(
        priceStr.data(), priceStr.data() + priceStr.size(), priceVal)};

    if (ec != std::errc{} || ptr != (priceStr.data() + priceStr.size())) {
      throw std::invalid_argument(
          std::format("Error: Invalid bond price: {}", std::string(priceStr)));
    }
    market.addMarketData(std::string(bondStr), BondPrice{priceVal});
  }
}

std::vector<std::unique_ptr<ITrade>>
loadTrades(const std::filesystem::path &filePath) {
  std::vector<std::unique_ptr<ITrade>> portfolio{};
  std::ifstream file{filePath};

  if (!file.is_open()) {
    throw std::runtime_error(
        std::format("Error: Could not open trade file: {}", filePath.string()));
  }

  std::string line{};
  // Skip the header line and reset file's internal pointer
  if (std::getline(file, line)) {
    if (line.find("id;") == std::string::npos) { // not a header line
      file.clear();                              // clear EOF flag if set
      file.seekg(0);
    }
  }

  while (std::getline(file, line)) {
    std::string_view lineView{trimView(line)};
    if (lineView.empty())
      continue;

    auto values{splitView(lineView, ';')};
    if (values.size() < 11)
      continue;

    TradeRecord tradeRecord{};

    // ID
    std::from_chars_result result{};
    std::string_view tmpView{trimView(values[0])};
    result = std::from_chars(tmpView.data(), tmpView.data() + tmpView.size(),
                             tradeRecord.id);
    if (result.ec != std::errc{} ||
        result.ptr != tmpView.data() + tmpView.size()) {
      throw std::invalid_argument(std::format("Error: Invalid ID"));
    }

    // Trade type
    std::string type{std::string(trimView(values[1]))};
    std::ranges::transform(type, type.begin(),
                           [](unsigned char c) { return std::tolower(c); });
    if (type == "swap") {
      tradeRecord.tradeType = TradeType::Swap;
    } else if (type == "bond") {
      tradeRecord.tradeType = TradeType::Bond;
    } else if (type == "european" || type == "american") {
      tradeRecord.tradeType = TradeType::Option;
      if (type == "european") {
        tradeRecord.optionExerciseStyle = OptionExerciseStyle::European;
      } else {
        tradeRecord.optionExerciseStyle = OptionExerciseStyle::American;
      }
    } else {
      throw std::invalid_argument(
          std::format("Error: Invalid Trade Type: {}", type));
    }

    // Trade date
    tradeRecord.tradeDate = parseDate(trimView(values[2]));

    // Start date
    tradeRecord.startDate = parseDate(trimView(values[3]));

    // End date
    tradeRecord.endDate = parseDate(trimView(values[4]));

    // Notional
    tmpView = trimView(values[5]);
    result = std::from_chars(tmpView.data(), tmpView.data() + tmpView.size(),
                             tradeRecord.notional);
    if (result.ec != std::errc{} ||
        result.ptr != tmpView.data() + tmpView.size()) {
      throw std::invalid_argument(std::format("Error: Invalid Notional"));
    }

    // Instrument name
    tradeRecord.name = std::string(trimView(values[6]));

    // Rate
    tmpView = trimView(values[7]);
    result = std::from_chars(tmpView.data(), tmpView.data() + tmpView.size(),
                             tradeRecord.rate);
    if (result.ec != std::errc{} ||
        result.ptr != tmpView.data() + tmpView.size()) {
      throw std::invalid_argument(std::format("Error: Invalid Rate"));
    }

    // Strike
    tmpView = trimView(values[8]);
    result = std::from_chars(tmpView.data(), tmpView.data() + tmpView.size(),
                             tradeRecord.strike);
    if (result.ec != std::errc{} ||
        result.ptr != tmpView.data() + tmpView.size()) {
      throw std::invalid_argument(std::format("Error: Invalid Strike"));
    }

    // Frequency
    tmpView = trimView(values[9]);
    result = std::from_chars(tmpView.data(), tmpView.data() + tmpView.size(),
                             tradeRecord.frequency);
    if (result.ec != std::errc{} ||
        result.ptr != tmpView.data() + tmpView.size()) {
      throw std::invalid_argument(std::format("Error: Invalid Frequency"));
    }

    // Option right
    std::string optionRight{std::string(trimView(values[10]))};
    std::ranges::transform(optionRight, optionRight.begin(),
                           [](unsigned char c) { return std::tolower(c); });
    if (optionRight == "call") {
      tradeRecord.optionRight = OptionRight::Call;
    } else if (optionRight == "put") {
      tradeRecord.optionRight = OptionRight::Put;
    } else if (optionRight == "na") {
      tradeRecord.optionRight = OptionRight::NA;
    } else {
      throw std::invalid_argument(
          std::format("Error: Invalid Option Right: {}", optionRight));
    }

    // Currency
    std::string ccy{std::string(trimView(values[11]))};
    std::ranges::transform(ccy, ccy.begin(),
                           [](unsigned char c) { return std::tolower(c); });
    if (ccy == "usd") {
      tradeRecord.tradeCcy = Currency::USD;
    } else {
      throw std::invalid_argument(
          std::format("Error: Invalid Currency: {}", ccy));
    }

    // Option payoff (if option field != na)
    std::string payoff{std::string(trimView(values[12]))};
    std::ranges::transform(payoff, payoff.begin(),
                           [](unsigned char c) { return std::tolower(c); });
    if (payoff == "vanilla") {
      tradeRecord.optionPayoff = OptionPayoff::Vanilla;
    } else if (payoff == "binary") {
      tradeRecord.optionPayoff = OptionPayoff::Binary;
    } else if (payoff == "callspread") {
      tradeRecord.optionPayoff = OptionPayoff::CallSpread;
    } else if (payoff == "na") {
      tradeRecord.optionPayoff = OptionPayoff::NA;
    } else {
      throw std::invalid_argument(
          std::format("Error: Invalid Option Payoff: {}", payoff));
    }

    // Instantiate trade object and put into portfolio vector
    auto trade{TradeFactory::createTrade(tradeRecord)};
    if (trade) {
      portfolio.push_back(std::move(trade));
    }
  }

  return portfolio;
}
