cmake_minimum_required(VERSION 3.7.0)
project(adventofcode)

# ########## dependencies from conan #########
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

find_package(Catch2 REQUIRED)

# ########## modern C++ flags######################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ########## advent of Application ###################

add_executable(adventofcode day01.cpp day02.cpp day03.cpp day04.cpp main.cpp)
target_link_libraries(adventofcode Catch2::Catch2)
target_compile_definitions(adventofcode
                           PUBLIC DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
