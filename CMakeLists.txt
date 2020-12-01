cmake_minimum_required(VERSION 3.7.0)
project(adventofcode)

# ########## dependencies from conan #########
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

find_package(debug_assert REQUIRED)

# ########## modern C++ flags######################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ########## Tetris Application ###################

add_executable(adventofcode main.cpp)
target_link_libraries(adventofcode debug_assert::debug_assert)
target_compile_definitions(adventofcode
                           PUBLIC DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

# test ########
if(MINESWEEPER_WITH_DEVELOPMENT_DEPENDANCIES)
  find_package(Catch2 REQUIRED)

  add_executable(test_minesweeper test/test_board.cpp test/test_generator.cpp
                                  test/test_game_logic.cpp test/main_catch.cpp)
  target_compile_definitions(
    test_minesweeper PUBLIC UNITEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test")
  target_link_libraries(test_minesweeper Minesweeper::Minesweeper
                        Catch2::Catch2)

  # test coverage with kcov and ctest ######################

  find_program(KCOV kcov)
  message("kcov: ${KCOV}")
  if(KCOV)
    enable_testing()

    add_test(
      NAME minesweeper_coverage
      COMMAND
        ${KCOV} --exclude-pattern=/usr/
        --include-path=${CMAKE_CURRENT_LIST_DIR}/src cov
        $<TARGET_FILE:test_minesweeper>)
  endif()
endif()
