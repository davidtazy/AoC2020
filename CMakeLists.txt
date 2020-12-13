cmake_minimum_required(VERSION 3.7.0)
project(adventofcode)

# ########## dependencies from conan #########
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

find_package(Catch2 REQUIRED)
find_package(range-v3 REQUIRED)

# ########## modern C++ flags######################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ########## advent of Application ###################

add_executable(
  test_adventofcode
  day01.cpp
  day02.cpp
  day03.cpp
  day04.cpp
  day05.cpp
  day06.cpp
  day07.cpp
  day08.cpp
  day09.cpp
  day10.cpp
  day11.cpp
  day12.cpp
  day13.cpp
  main.cpp)
target_link_libraries(test_adventofcode Catch2::Catch2 range-v3::range-v3)
target_compile_definitions(test_adventofcode
                           PUBLIC DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

# #######################  range v3###################################
