#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <range/v3/all.hpp>  // get everything
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