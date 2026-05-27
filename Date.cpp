#include "Date.h"

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
double operator-(const Date &d1, const Date &d2) {
  int yearDiff = d1.getYear() - d2.getYear();
  int monthDiff = d1.getMonth() - d2.getMonth();
  int dayDiff = d1.getDay() - d2.getDay();

  // Perform floating point division instead of integer division (e.g. 12.0
  // or 365.0 instead of 12 or 365)
  return yearDiff + monthDiff / 12.0 + dayDiff / 365.0;
}
