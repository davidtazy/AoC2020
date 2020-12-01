#include <array>
#include <debug_assert.hpp>
#include <fstream>
#include <iostream>
#include <vector>

struct my_module : debug_assert::default_handler,  // use the default handler
                   debug_assert::set_level<-1>  // level -1, i.e. all assertions, 0 would mean none,
                                                // 1 would be level 1, 2 level 2 or lower,...
{};

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

int main(int argc, char** argv) {
  std::string input_file_01{DATA_DIR "/01/input.txt"};

  std::cout << "input file = " << input_file_01 << "\n";

  auto vect = read_inputs(input_file_01);

  std::cout << vect.size() << "\n";
  DEBUG_ASSERT(vect.size() > 0, my_module{});

  {
    auto [n1, n2] = find_first_sum(vect, 2020);

    std::cout << n1 << " x " << n2 << " = " << n1 * n2 << "\n";
  }

  {
    auto [n1, n2, n3] = find_first_sum_of_3(vect, 2020);

    std::cout << n1 << " x " << n2 << " x " << n3 << " = " << n1 * n2 * n3 << "\n";
  }

  return EXIT_SUCCESS;
}