#ifndef MARKET_DATA_LOADER_H
#define MARKET_DATA_LOADER_H

#include "Date.h"
#include <string_view>

// Helper function to convert tenors (e.g. 3M, 6M) to a Date object relative to
// now parameter
Date parseTenor(const Date &now, const std::string_view tenor);

// Helper function to parse values with percentage sign
double parsePercentage(const std::string_view value);

#endif
