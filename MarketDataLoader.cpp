#include "MarketDataLoader.h"
#include <charconv>
#include <stdexcept>
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
        std::format("Invalid numeric prefix in tenor: {}", tenor));
  }

  if (unit == 'D' || unit == 'd') {
    return now + (time / 360.0);
  } else if (unit == 'W' || unit == 'w') {
    return now + ((time * 7.0) / 360.0);
  } else if (unit == 'M' || unit == 'm') {
    return now + (time / 12.0);
  } else if (unit == 'Y' || unit == 'y') {
    return now + time;
  }

  throw std::invalid_argument(std::format("Unknown tenor: {}", tenor));
}
