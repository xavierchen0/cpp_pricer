#ifndef MARKET_DATA_LOADER_H
#define MARKET_DATA_LOADER_H

#include <filesystem>

// Functions to load data into Market object from files; Modify in-place
void loadRateCurve(const std::filesystem::path &filePath);
void loadVolCurve(const std::filesystem::path &filePath,
                  const std::string &curveName);
void loadStockPrices(const std::filesystem::path &filePath);
void loadBondPrices(const std::filesystem::path &filePath);

#endif
