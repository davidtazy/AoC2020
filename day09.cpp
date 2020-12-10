#include <array>
#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <range/v3/all.hpp>  // get everything
#include <set>
#include <sstream>
#include <variant>
#include <vector>

std::vector<unsigned long long> parse_numbers(std::istream& in) {
  return ranges::istream<unsigned long long>(in) | ranges::to_vector;
}

bool check_xmas(std::vector<unsigned long long> pre, unsigned long long val) {
  for (const auto& [index, a] : pre | ranges::views::enumerate) {
    for (int j : ranges::views::ints(index + 1, pre.size())) {
      assert(j < pre.size());
      auto b = pre.at(j);
      if (a == b)
        continue;
      if (a + b == val)
        return true;
    }
  }
  return false;
}

std::pair<int, unsigned long long> find_first_wrong_number(std::vector<unsigned long long> numbers,
                                                           int len) {
  for (int i : ranges::views::ints(len, static_cast<int>(numbers.size()))) {
    if (!check_xmas(numbers | ranges::view::slice(i - len, i) | ranges::to_vector, numbers.at(i))) {
      return {i, numbers.at(i)};
    }
  }
  throw std::runtime_error("cannot find wrong number");
}

std::pair<int, int> find_contigous(std::vector<unsigned long long> numbers,
                                   unsigned long long val) {
  for (int i : ranges::views::ints(0, static_cast<int>(numbers.size()))) {
    unsigned long long sum = 0;
    for (int j = i; j < numbers.size(); j++) {
      sum += numbers.at(j);
      if (sum == val) {
        return {i, j};
      }
      if (sum > val)
        continue;
    }
  }
  throw std::runtime_error("cannot find wrong number");
}

TEST_CASE("Day 9: Encoding Error") {
  std::string data(R"_(35
20
15
25
47
40
62
55
65
95
102
117
150
182
127
219
299
277
309
576)_");
  std::istringstream in{data};

  auto numbers = parse_numbers(in);

  SECTION("part 1") {
    REQUIRE(numbers.size() == 20);

    REQUIRE(numbers.at(0) == 35);
    REQUIRE(numbers.back() == 576);

    REQUIRE(check_xmas(numbers | ranges::view::slice(0, 5) | ranges::to_vector, numbers.at(5)));

    REQUIRE(find_first_wrong_number(numbers, 5).second == 127);
  }

  SECTION("part 2") {
    auto [min, max] =
        find_contigous(numbers | ranges::views::slice(0, 15) | ranges::to_vector, 127);

    REQUIRE(min == 2);
    REQUIRE(max == 5);
  }
};

TEST_CASE("day 9  ") {
  std::ifstream in(DATA_DIR "/dataset/input_09.txt", std::ifstream::in);
  REQUIRE(in.good());

  auto numbers = parse_numbers(in);

  auto [index, number] = find_first_wrong_number(numbers, 25);

  std::cout << " day 9 part 1 : " << number << "\n";

  auto [i, j] =
      find_contigous(numbers | ranges::views::slice(0, index) | ranges::to_vector, number);

  const auto [min, max] = ranges::minmax_element(numbers | ranges::view::slice(i, j + 1));

  std::cout << " day 9 part 2 : " << *min + *max << "\n";
}