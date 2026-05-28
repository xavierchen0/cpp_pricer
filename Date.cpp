#include "Date.h"

// Helper function to get date in years
namespace {

// Assume 30/360 day count convention
int getTotalDays(const Date &date) {
  // Handle the case of 31st day and make it 30th day to align with 30/360 day
  // count convention
  int day{date.getDay() > 30 ? 30 : date.getDay()};
  return date.getYear() * 360 + (date.getMonth() - 1) * 30 + (day - 1);
}

} // namespace

std::istream &operator>>(std::istream &is, Date &date) {
  int year{};
  int month{};
  int day{};
  char dash1{};
  char dash2{};

  if (is >> year >> dash1 >> month >> dash2 >> day) {
    if (dash1 != '-' || dash2 != '-' || month <= 0 || month >= 13 || day <= 0 ||
        day >= 32) {
      is.setstate(std::ios_base::failbit);
    }
  }

  // Copy assign referenced Date object if input extraction successful;
  // otherwise, copy assigns a default-initialised Date as fallback
  date = is ? Date{year, month, day} : Date{};

  return is;
}

std::ostream &operator<<(std::ostream &os, const Date &d) {
  os << d.getYear() << "-" << d.getMonth() << "-" << d.getDay();
  return os;
}

// return date difference in fraction of year
double operator-(const Date &date1, const Date &date2) {
  return (getTotalDays(date1) - getTotalDays(date2)) / 360.0;
}
