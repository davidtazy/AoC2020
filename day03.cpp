#include <array>
#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using Forest = std::vector<std::string>;

Forest read_forest(std::istream& in) {
  Forest forest;
  while (in.good()) {
    std::string line;
    in >> line;
    forest.push_back(line);
  }
  return forest;
}

int count_trees_with_slope(int slope, int speed, const Forest& forest) {
  int count = 0;
  int col = 0;
  int len = forest.front().size();
  for (int row = 0; row < forest.size(); row += speed) {
    const auto& line = forest.at(row);
    if (line.at(col) == '#') {
      count++;
    }
    col += slope;
    col %= len;
  }

  return count;
}
#include <cstdint>

uint64_t count_and_multiply_slopes(const Forest& forest) {
  uint64_t ret = 1;
  using p = std::pair<int, int>;
  for (auto [right, down] : {p{1, 1}, p{3, 1}, p{5, 1}, p{7, 1}, p{1, 2}}) {
    ret *= count_trees_with_slope(right, down, forest);
  }
  return ret;
}

TEST_CASE("Toboggan Trajectory example") {
  std::stringstream in(R"_(..##.......
  #...#...#..
  .#....#..#.
  ..#.#...#.#
  .#...##..#.
  ..#.##.....
  .#.#.#....#
  .#........#
  #.##...#...
  #...##....#
  .#..#...#.#)_");

  auto forest = read_forest(in);
  REQUIRE(forest.size() == 11);
  REQUIRE(forest.front() == "..##.......");

  REQUIRE(count_trees_with_slope(3, 1, forest) == 7);

  REQUIRE(count_and_multiply_slopes(forest) == 336);
}

TEST_CASE("day 3  ") {
  std::ifstream in(DATA_DIR "/dataset/input_03.txt", std::ifstream::in);
  REQUIRE(in.good());

  auto forest = read_forest(in);

  std::cout << " day 3 part 1 : " << count_trees_with_slope(3, 1, forest) << "\n";
  std::cout << " day 3 part 2 : " << count_and_multiply_slopes(forest) << "\n";
}