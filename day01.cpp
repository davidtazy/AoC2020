
#include <array>
#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

std::vector<int> read_inputs(std::string file) {
  std::ifstream in(file, std::ifstream::in);

  std::vector<int> ret;

  while (in.good()) {
    int value;
    in >> value;
    ret.push_back(value);
  }
  return ret;
}

std::pair<int, int> find_first_sum(const std::vector<int> values, int sum) {
  for (int i = 0; i < values.size(); i++) {
    for (int j = i + 1; j < values.size(); j++) {
      if (values.at(i) + values.at(j) == sum) {
        return {values.at(i), values.at(j)};
      }
    }
  }

  throw std::runtime_error("cannot find sum");
}

std::array<int, 3> find_first_sum_of_3(const std::vector<int> values, int sum) {
  for (int i = 0; i < values.size(); i++) {
    for (int j = i + 1; j < values.size(); j++) {
      for (int k = j + 1; k < values.size(); k++) {
        if (values.at(i) + values.at(j) + values.at(k) == sum) {
          return {values.at(i), values.at(j), values.at(k)};
        }
      }
    }
  }

  throw std::runtime_error("cannot find sum");
}

TEST_CASE("day 1") {
  std::string input_file_01{DATA_DIR "/dataset/input_01.txt"};

  auto vect = read_inputs(input_file_01);

  REQUIRE(vect.size() == 200);

  {
    auto [n1, n2] = find_first_sum(vect, 2020);

    std::cout << " day 1 part 1 : " << n1 * n2 << "\n";
  }

  {
    auto [n1, n2, n3] = find_first_sum_of_3(vect, 2020);

    std::cout << " day 1 part 2 : " << n1 * n2 * n3 << "\n";
  }
}
