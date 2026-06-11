#ifndef MARKET_DATA_LOADER_H
#define MARKET_DATA_LOADER_H

#include "instruments/Trade.h"
#include <filesystem>
#include <memory>
#include <vector>

// Functions to load data into Market object from files; Modify in-place
void loadRateCurve(const std::filesystem::path &filePath, class Market &market);
void loadVolCurve(const std::filesystem::path &filePath,
                  const std::string &curveName, class Market &market);
void loadStockPrices(const std::filesystem::path &filePath,
                     class Market &market);
void loadBondPrices(const std::filesystem::path &filePath,
                    class Market &market);

// Functions to load trade data into Trade factory
std::vector<std::unique_ptr<ITrade>>
loadTrades(const std::filesystem::path &filepath);

#endif
