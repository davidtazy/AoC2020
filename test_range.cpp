#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <range/v3/all.hpp>  // get everything
#include <sstream>
#include <string>
using std::cout;

struct PasswordLine {
  int min;
  int max;
  char ch;
  std::string password;

  explicit PasswordLine(const std::string& line) {
    std::istringstream in(line);
    in >> min;
    in.ignore();
    in >> max >> ch;
    in.ignore();
    in >> password;
  }
};

bool is_valid_password(const PasswordLine& row) {
  int nb_occurrence = std::count(row.password.begin(), row.password.end(), row.ch);
  return row.min <= nb_occurrence && nb_occurrence <= row.max;
}

TEST_CASE("day 02 with range-v3") {
  std::istringstream in{R"__(5-8 c: cccqfccccccc
8-14 k: xnjcftlkvhkmkr
4-5 q: qqqqqj)__"};

  auto valid_passwords =
      ranges::getlines_view(in) |
      ranges::views::transform([](std::string str) { return PasswordLine(str); }) |
      ranges::views::filter(is_valid_password) | ranges::to<std::vector>();

  REQUIRE(valid_passwords.size() == 1);
}
#include <string>

int count_sum_of_yes_of_groups(const std::string& str) {
  using namespace std::string_literals;
  auto numbers_of_yes_by_group =
      str | ranges::view::split("\n\n"s) |      // split by group
      ranges::view::transform([](auto group) {  // keep all yes
        return group |                          //
               ranges::view::split("\n"s) |     //
               ranges::actions::join |          //
               ranges::actions::sort |          // unique works with sorted containers
               ranges::actions::unique |        //
               ranges::to<std::string>;         // why?
      }) |
      ranges::view::transform([](auto group) {
        return group.size();  //  number of yes
      });

  return ranges::accumulate(numbers_of_yes_by_group, 0);
}

int count_sum_of_yes_of_groups(std::istream& in) {
  using namespace std::string_literals;
  auto numbers_of_yes_by_group =
      ranges::getlines(in) |                      //
      ranges::view::split(""s) |                  //
      ranges::view::transform([](auto answers) {  //
        return answers |                          //
               ranges::view::join |               //
               ranges::to<std::vector>() |        //
               ranges::actions::sort |            // unique works with sorted containers
               ranges::actions::unique;           //
      }) |
      ranges::view::transform([](auto group) {
        return group.size();  //  number of yes
      });

  // std::cout << numbers_of_yes_by_group << '\n'; // NOOOOOO istream is consumed once!
  return ranges::accumulate(numbers_of_yes_by_group, 0);
}

TEST_CASE("test split string") {
  std::string str = R"_(abc

a
b
c

ab
ac

a
a
a
a

b)_";

  std::cout << count_sum_of_yes_of_groups(str) << '\n';

  std::istringstream in{str};

  std::cout << count_sum_of_yes_of_groups(in) << '\n';
}
