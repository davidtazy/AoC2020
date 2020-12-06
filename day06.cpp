#include <array>
#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <range/v3/all.hpp>  // get everything
#include <set>
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

auto unanimous_answers_from_group(const std::vector<std::string>& group) {
  std::string first = group.front();

  std::set<char> ret;

  for (char c : group.front()) {
    bool contain = true;
    for (auto str : group) {
      if (ranges::count(str, c) == 0) {
        contain = false;
        break;
      }
    }
    if (contain) {
      ret.insert(c);
    }
  }

  return ret | ranges::to<std::string>();
}

int count_sum_of_unanimous_yes_of_groups(std::istream& in) {
  using namespace std::string_literals;
  auto numbers_of_unanimous_yes_by_group = ranges::getlines(in) |                      //
                                           ranges::view::split(""s) |                  //
                                           ranges::view::transform([](auto answers) {  //
                                             auto vect = answers | ranges::to<std::vector>();
                                             return unanimous_answers_from_group(vect);
                                           }) |
                                           ranges::view::transform([](auto group) {
                                             return group.size();  //  number of yes
                                           });

  return ranges::accumulate(numbers_of_unanimous_yes_by_group, 0);
}

TEST_CASE("Custom Customs example") {
  std::string data(R"_(abc

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
  std::istringstream in1{data}, in2(data);
  REQUIRE(count_sum_of_yes_of_groups(in1) == 11);
  REQUIRE(count_sum_of_unanimous_yes_of_groups(in2) == 6);
}

TEST_CASE("day 6  ") {
  std::ifstream in(DATA_DIR "/dataset/input_06.txt", std::ifstream::in);
  std::ifstream in2(DATA_DIR "/dataset/input_06.txt", std::ifstream::in);
  REQUIRE(in.good());

  std::cout << " day 6 part 1 : " << count_sum_of_yes_of_groups(in) << "\n";
  std::cout << " day 6 part 2 : " << count_sum_of_unanimous_yes_of_groups(in2) << "\n";
}