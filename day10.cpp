#include <array>
#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <range/v3/all.hpp>  // get everything
#include <set>
#include <sstream>
#include <variant>
#include <vector>

std::vector<unsigned long long> parse_adapters_and_sort(std::istream& in) {
  std::vector<unsigned long long> first = {0};
  auto numbers =
      ranges::views::concat(ranges::views::all(first), ranges::istream<unsigned long long>(in)) |
      ranges::to_vector | ranges::actions::sort;

  numbers.push_back(numbers.back() + 3);

  return numbers;
}

std::tuple<int, int> diff_and_find_number_of_1_and_3(
    const std::vector<unsigned long long>& numbers) {
  int number_of_1 = 0;
  int number_of_3 = 0;
  for (int i : ranges::views::iota(1, (int)numbers.size())) {
    auto dif = numbers.at(i) - numbers.at(i - 1);
    if (dif == 1)
      number_of_1++;
    if (dif == 3)
      number_of_3++;
  }
  return {number_of_1, number_of_3};
}

template <typename T>
bool is_valid(T& slice) {
  // only continuous values
  return ranges::adjacent_find(slice, [](auto a, auto b) { return b - a != 1; }) == slice.end();
}

template <typename T>
int count_possibilities(T& slice) {
  assert(is_valid(slice));

  switch (slice.size()) {
    case 3:
      return 2;
    case 4:
      return 4;
    case 5:
      return 7;
    default:
      throw std::runtime_error("dont know ");
  }

  return 0;
}

unsigned long long count_all_possibilities(std::vector<unsigned long long> numbers) {
  int min = 0;

  unsigned long long ret = 1;

  for (int i : ranges::views::iota(1, (int)numbers.size())) {
    auto dif = numbers.at(i) - numbers.at(i - 1);

    if (dif == 3) {
      auto slice = numbers | ranges::views::slice(min, i);
      min = i;
      if (slice.size() <= 2)
        continue;
      // std::cout << ranges::views::all(slice) << ' ';
      ret *= count_possibilities(slice);
    }
  }
  return ret;
}

TEST_CASE("Day 10: Adapter Array") {
  std::string data(R"_(16
10
15
5
1
11
7
19
6
12
4)_");
  std::istringstream in{data};

  auto numbers = parse_adapters_and_sort(in);

  SECTION("part 1") {
    auto [number_of_one, number_of_three] = diff_and_find_number_of_1_and_3(numbers);

    REQUIRE(number_of_one == 7);
    REQUIRE(number_of_three == 5);
  }

  SECTION("part 2") {
    REQUIRE(count_all_possibilities(numbers) == 8);
    std::string data2(R"_(28
33
18
42
31
14
46
20
48
47
24
23
49
45
19
38
39
11
1
32
25
35
8
17
7
9
4
2
34
10
3)_");
    std::istringstream in2{data2};

    auto numbers2 = parse_adapters_and_sort(in2);

    REQUIRE(count_all_possibilities(numbers2) == 19208);
  };
};

TEST_CASE("day 10  ") {
  std::ifstream in(DATA_DIR "/dataset/input_10.txt", std::ifstream::in);
  REQUIRE(in.good());

  auto numbers = parse_adapters_and_sort(in);
  auto [number_of_one, number_of_three] = diff_and_find_number_of_1_and_3(numbers);

  std::cout << " day 10 part 1 : " << number_of_one * number_of_three << "\n";

  std::cout << " day 10 part 2 : " << count_all_possibilities(numbers) << "\n";
}