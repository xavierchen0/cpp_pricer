#ifndef MARKET_DATA_LOADER_H
#define MARKET_DATA_LOADER_H

#include "Date.h"
#include <string>
#include <string_view>

// Helper function to convert tenors (e.g. 3M, 6M) to a Date object relative to
// now parameter
Date parseTenor(const Date &now, const std::string_view tenor);

// Helper function to parse values with percentage sign
double parsePercentage(const std::string_view value);

// Helper function to trim leading and trailing whitespaces away in string
std::string trim(const std::string_view str);

#endif
