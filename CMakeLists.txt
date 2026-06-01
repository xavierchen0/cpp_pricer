cmake_minimum_required(VERSION 3.23)
project(cpp_pricer)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(cpp_pricer)

target_sources(cpp_pricer
  PRIVATE
    Date.cpp
    Market.cpp
    MarketDataLoader.cpp
    Main.cpp
)

target_compile_features(cpp_pricer
  PRIVATE
    cxx_std_23
)

target_compile_options(cpp_pricer
  PRIVATE
    -Wall
    -Weffc++
    -Wextra
    -Wconversion
    -Wsign-conversion
    -Werror
)
