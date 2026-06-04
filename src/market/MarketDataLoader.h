#ifndef MARKET_DATA_LOADER_H
#define MARKET_DATA_LOADER_H

#include "core/Date.h"
#include <filesystem>
#include <string_view>

// Helper function to convert tenors (e.g. 3M, 6M) to a Date object relative to
// now parameter
Date parseTenor(const Date &now, const std::string_view tenor);

// Helper function to parse values with percentage sign
double parsePercentage(const std::string_view value);

// Helper function to trim leading and trailing whitespaces away in string
std::string_view trimView(const std::string_view str);

// Functions to load data into Market object from files; Modify in-place
void loadRateCurve(const std::filesystem::path &filePath);
void loadVolCurve(const std::filesystem::path &filePath,
                  const std::string &curveName);
void loadStockPrices(const std::filesystem::path &filePath);
void loadBondPrices(const std::filesystem::path &filePath);

#endif
