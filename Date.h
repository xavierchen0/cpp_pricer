#ifndef DATE_H
#define DATE_H

#include <istream>
#include <ostream>

/*
   Date object is Value object so the following design enforces the
   "Immutability" property of this object. The class only exposes getter
   functions for data retrieval, but only allow modification via operator>>
   when reading dates from external data streams, such as files or user input.
   This localised mutability eliminates the need for public setter functions,
   protecting the object from arbitrary modifications elsewhere in the
   application after its initial creation.
*/
class Date {
public:
  Date() = default;

  Date(int year, int month, int day)
      : m_year{year}, m_month{month}, m_day{day} {}

  int getYear() const { return m_year; }
  int getMonth() const { return m_month; }
  int getDay() const { return m_day; }

  friend std::istream &operator>>(std::istream &is, Date &date);

private:
  int m_year{};
  int m_month{};
  int m_day{};
};

// Output
// Prints Date as YYYY-MM-DD string format
std::ostream &operator<<(std::ostream &os, const Date &date);

// Arithmetic
// Calculate difference between two Date objects in fraction of years
double operator-(const Date &date1, const Date &date2);
// Initialise new Date object by adding fraction of year to initial Date object
Date operator+(const Date &date1, double year_frac);


#endif
