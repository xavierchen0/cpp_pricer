#include "MarketDataLoader.h"
#include <charconv>
#include <format>
#include <stdexcept>
#include <string_view>
#include <system_error>

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

std::string_view trimView(const std::string_view str) {
  constexpr std::string_view whitespace{" \t\r\n"};

  const auto first{str.find_first_not_of(whitespace)};

  if (first == std::string_view::npos) { // str only contains whitespace
    return {};
  }

  const auto last{str.find_last_not_of(whitespace)};

  return str.substr(first, last - first + 1);
}
