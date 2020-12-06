#include <array>
#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <range/v3/all.hpp>  // get everything
#include <sstream>
#include <vector>

int count_sum_of_yes_of_groups(std::istream& in) {
  using namespace std::string_literals;
  auto numbers_of_yes_by_group =
      ranges::getlines(in) |                      //
      ranges::view::split(""s) |                  //
      ranges::view::transform([](auto answers) {  //
        return answers |                          //
               ranges::view::join |               //
               ranges::to<std::vector>() |        // sort can not operate on views
               ranges::actions::sort |            // unique works with sorted containers
               ranges::actions::unique;           //
      }) |
      ranges::view::transform([](auto group) {
        return group.size();  //  number of yes
      });

  // std::cout << numbers_of_yes_by_group << '\n'; // NOOOOOO istream is consumed once!
  return ranges::accumulate(numbers_of_yes_by_group, 0);
}

TEST_CASE("Custom Customs example") {
  std::stringstream in(R"_(abc

a
b
c

ab
ac

a
a
a
a

b)_");

  REQUIRE(count_sum_of_yes_of_groups(in) == 11);
}

TEST_CASE("day 6  ") {
  std::ifstream in(DATA_DIR "/dataset/input_06.txt", std::ifstream::in);
  REQUIRE(in.good());

  std::cout << " day 6 part 1 : " << count_sum_of_yes_of_groups(in) << "\n";
  // std::cout << " day 3 part 2 : " << count_and_multiply_slopes(forest) << "\n";
}