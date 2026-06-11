#include "core/Date.h"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace {

int getTotalDays(const Date &date) {
  std::chrono::year_month_day ymd{
      std::chrono::year{date.getYear()},
      std::chrono::month{static_cast<unsigned>(date.getMonth())},
      std::chrono::day{static_cast<unsigned>(date.getDay())}};
  std::chrono::sys_days sd = ymd;
  return sd.time_since_epoch().count();
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

  date = is ? Date{year, month, day} : Date{};

  return is;
}

std::ostream &operator<<(std::ostream &os, const Date &d) {
  std::ostringstream tempstream{};
  tempstream << d.getYear() << "-" << std::setfill('0') << std::setw(2)
             << d.getMonth() << "-" << std::setfill('0') << std::setw(2)
             << d.getDay();

  os << tempstream.str();

  return os;
}

double operator-(const Date &date1, const Date &date2) {
  return (getTotalDays(date1) - getTotalDays(date2)) / 365.0;
}

Date &Date::operator-=(double year_frac) {
  int daysToSubtract{static_cast<int>(std::round(year_frac * 365.0))};
  std::chrono::year_month_day ymd{
      std::chrono::year{m_year},
      std::chrono::month{static_cast<unsigned>(m_month)},
      std::chrono::day{static_cast<unsigned>(m_day)}};
  std::chrono::sys_days sd =
      std::chrono::sys_days(ymd) - std::chrono::days(daysToSubtract);
  std::chrono::year_month_day new_ymd{sd};

  m_year = static_cast<int>(new_ymd.year());
  m_month = static_cast<int>(static_cast<unsigned>(new_ymd.month()));
  m_day = static_cast<int>(static_cast<unsigned>(new_ymd.day()));

  return *this;
}

Date operator+(const Date &date, double year_frac) {
  int daysToAdd{static_cast<int>(std::round(year_frac * 365.0))};
  std::chrono::year_month_day ymd{
      std::chrono::year{date.getYear()},
      std::chrono::month{static_cast<unsigned>(date.getMonth())},
      std::chrono::day{static_cast<unsigned>(date.getDay())}};
  std::chrono::sys_days sd =
      std::chrono::sys_days(ymd) + std::chrono::days(daysToAdd);
  std::chrono::year_month_day new_ymd{sd};

  return Date{static_cast<int>(new_ymd.year()),
              static_cast<int>(static_cast<unsigned>(new_ymd.month())),
              static_cast<int>(static_cast<unsigned>(new_ymd.day()))};
}

Date &Date::operator+=(double year_frac) {
  int daysToAdd{static_cast<int>(std::round(year_frac * 365.0))};
  std::chrono::year_month_day ymd{
      std::chrono::year{m_year},
      std::chrono::month{static_cast<unsigned>(m_month)},
      std::chrono::day{static_cast<unsigned>(m_day)}};
  std::chrono::sys_days sd =
      std::chrono::sys_days(ymd) + std::chrono::days(daysToAdd);
  std::chrono::year_month_day new_ymd{sd};

  m_year = static_cast<int>(new_ymd.year());
  m_month = static_cast<int>(static_cast<unsigned>(new_ymd.month()));
  m_day = static_cast<int>(static_cast<unsigned>(new_ymd.day()));

  return *this;
}

bool operator==(const Date &date1, const Date &date2) {
  return date1.getYear() == date2.getYear() &&
         date1.getMonth() == date2.getMonth() &&
         date1.getDay() == date2.getDay();
}

bool operator!=(const Date &date1, const Date &date2) {
  return !(date1 == date2);
}

bool operator<(const Date &date1, const Date &date2) {
  if (date1.getYear() != date2.getYear())
    return date1.getYear() < date2.getYear();
  if (date1.getMonth() != date2.getMonth())
    return date1.getMonth() < date2.getMonth();
  return date1.getDay() < date2.getDay();
}

bool operator>(const Date &date1, const Date &date2) { return date2 < date1; }

bool operator<=(const Date &date1, const Date &date2) {
  return !(date1 > date2);
}

bool operator>=(const Date &date1, const Date &date2) {
  return !(date1 < date2);
}
