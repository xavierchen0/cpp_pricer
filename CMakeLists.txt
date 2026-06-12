cmake_minimum_required(VERSION 3.23)
project(cpp_pricer)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(cpp_pricer)

target_sources(cpp_pricer
  PRIVATE
    src/core/Date.cpp
    src/market/Market.cpp
    src/market/Loaders.cpp
    src/engine/Pricer.cpp
    src/engine/RiskEngine.cpp
    src/Main.cpp
)

target_include_directories(cpp_pricer
  PRIVATE
    src
)

target_compile_features(cpp_pricer
  PRIVATE
    cxx_std_23
)

# Conditional compiler flags based on the compiler being used
if(MSVC)
  target_compile_options(cpp_pricer
    PRIVATE
      /W4     # High warning level (similar to -Wall -Wextra)
      /WX     # Treat warnings as errors (equivalent to -Werror)
  )
else()
  # For Clang and GCC
  target_compile_options(cpp_pricer
    PRIVATE
      -Wall
      -Weffc++
      -Wextra
      -Wconversion
      -Wsign-conversion
      -Werror
  )
endif()

# Copy the data folder to the build directory after every successful build
add_custom_command(TARGET cpp_pricer POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/data
  ${CMAKE_CURRENT_BINARY_DIR}/data
)
