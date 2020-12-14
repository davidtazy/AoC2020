#include <array>
#include <bitset>
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
using namespace std::literals::string_literals;

using Bitset = std::bitset<36>;

struct Mask {
  explicit Mask(std::string mask) : mask(mask) {
    if (mask.size() != 36)
      throw std::runtime_error("mask is not expected size"s + std::to_string(mask.size()) +
                               "!= 36"s);
    and_bitset.set();
    for (auto [index, bit] : mask | ranges::view::reverse | ranges::view::enumerate) {
      if (bit == '1') {
        or_bitset.set(index);
      }
      if (bit == '0') {
        and_bitset.reset(index);
      }
    }
  }

  unsigned long apply(unsigned long value) const {
    Bitset v{value};

    v |= or_bitset;
    v &= and_bitset;

    return v.to_ulong();
  }

  std::string to_floating_mask(unsigned long address) const {
    std::string r = Bitset(address).to_string();
    auto addr = r | ranges::view::reverse | ranges::to<std::string>();

    // create floating mask
    for (auto [index, bit] : mask | ranges::view::reverse | ranges::view::enumerate) {
      if (bit == '1') {
        addr[index] = '1';
      }
      if (bit == 'X') {
        addr[index] = 'X';
      }
    }
    return addr | ranges::view::reverse | ranges::to<std::string>();
  }

  std::map<int, int> map_floating_mask(const std::string mask) const {
    std::map<int, int> map;
    int offset = 0;
    for (auto [index, c] : mask | ranges::view::enumerate) {
      if (c == 'X') {
        map[offset] = index;
        offset++;
      }
    }
    return map;
  }

  std::vector<unsigned long> addresses(unsigned long address) const {
    auto fl_mask = to_floating_mask(address);
    unsigned long nb_of_x = ranges::count(fl_mask, 'X');

    unsigned long max = 1ul << nb_of_x;
    // std::cout << "nb addresses= " << max << '\n';

    auto map = map_floating_mask(fl_mask);

    return ranges::view::iota(0ul, max) |
           ranges::view::transform([map, nb_of_x, fl_mask](unsigned long v) {
             std::string mask = fl_mask;
             Bitset counter{v};
             for (auto [counter_index, mask_index] : map) {
               // assert(mask.at(mask_index) == 'X');
               mask[mask_index] = (counter.test(counter_index)) ? '1' : '0';
             }

             return Bitset{mask}.to_ulong();
           }) |  //
           ranges::to_vector;
  }

  Bitset and_bitset;
  Bitset or_bitset;
  std::string mask;
};

struct Operations {
  explicit Operations(Mask m) : mask(m) {}

  struct Op {
    int address;
    unsigned long value;
  };
  Mask mask;
  std::vector<Op> ops;
};

struct Registry {
  void process(const std::vector<Operations>& ops_batch) {
    for (const Operations& ops : ops_batch) {
      const auto& mask = ops.mask;
      for (const Operations::Op& op : ops.ops) {
        this->map[op.address] = mask.apply(op.value);
      };
    };
  }

  void process_part2(const std::vector<Operations>& ops_batch) {
    for (const Operations& ops : ops_batch) {
      const auto& mask = ops.mask;
      for (const Operations::Op& op : ops.ops) {
        auto addresses = mask.addresses(op.address);
        // std::cout << addresses.size() << '\n';
        for (auto address : addresses) {
          this->map[address] = op.value;
        }
      }
    }
  }

  unsigned long long sum() { return ranges::accumulate(map | ranges::view::values, 0ull); }

  std::map<int, unsigned long> map;
};

std::vector<Operations> read_operations(std::istream& in) {
  std::vector<Operations> ops;

  while (in.good()) {
    std::string line;
    std::getline(in, line);

    auto mask =
        (line | ranges::view::split(" = "s) | ranges::to<std::vector<std::string>>()).back();

    auto& op = ops.emplace_back(Mask(mask));

    while (in.good()) {
      in.ignore();
      char c = in.peek();
      if (c == 'a') {
        // mask line
        break;
      }
      in.ignore(3);  // 'em['

      int adress;
      in >> adress;

      in.ignore(4);  // '] = '
      unsigned long val;
      in >> val;
      in.ignore();

      op.ops.push_back({adress, val});
    }
  }
  return ops;
}

TEST_CASE("Day 14: Docking Data example"){

    SECTION("read input"){

        std::string data(R"_(mask = XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X
mem[8] = 11
mem[7] = 101
mem[8] = 0
mask = XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X
mem[8] = 11
mem[7] = 101
mem[8] = 0)_");
std::istringstream in{data};

auto ops = read_operations(in);
REQUIRE(ops.size() == 2);
REQUIRE(ops.front().ops.size() == 3);
REQUIRE(ops.front().ops.front().address == 8);
REQUIRE(ops.front().ops.front().value == 11);

Registry reg;
reg.process(ops);
REQUIRE(reg.map.size() == 2);

REQUIRE(reg.sum() == 165);
}
SECTION("create mask from string") {
  Mask m{"XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X"s};

  REQUIRE(m.or_bitset == Bitset{"1000000"s});
  REQUIRE(m.and_bitset == Bitset{"111111111111111111111111111111111101"s});

  REQUIRE(m.apply(11) == 73);
  REQUIRE(m.apply(101) == 101);
  REQUIRE(m.apply(0) == 64);
}
}
;

TEST_CASE("day 14 part 1 ") {
  std::ifstream in(DATA_DIR "/dataset/input_14.txt", std::ifstream::in);
  REQUIRE(in.good());

  auto ops_batch = read_operations(in);

  Registry reg;

  reg.process(ops_batch);
  std::cout << " day 14 part 1 : " << reg.sum() << "\n";
}

TEST_CASE(" generate addresses") {
  Mask m{"000000000000000000000000000000X1001X"s};

  const int address = 42;

  REQUIRE(m.to_floating_mask(address) == "000000000000000000000000000000X1101X"s);
  REQUIRE(ranges::count("0X0X1X000"s, 'X') == 3);
  REQUIRE(1 << 4 == 16);
  REQUIRE(m.map_floating_mask("000000000000000000000000000000X1101X"s) ==
          std::map<int, int>{{0, 30}, {1, 35}});

  std::vector<unsigned long> aa = m.addresses(address);
  REQUIRE(aa == std::vector<unsigned long>{26, 58, 27, 59});
}

TEST_CASE(" day 14 part2 example ") {
  std::istringstream in(R"__(mask = 000000000000000000000000000000X1001X
mem[42] = 100
mask = 00000000000000000000000000000000X0XX
mem[26] = 1)__");

  auto ops = read_operations(in);
  Registry reg;

  reg.process_part2(ops);
  REQUIRE(reg.sum() == 208);
}

TEST_CASE("day 14 part2 ") {
  std::ifstream in(DATA_DIR "/dataset/input_14.txt", std::ifstream::in);
  REQUIRE(in.good());

  auto ops = read_operations(in);
  Registry reg;

  reg.process_part2(ops);

  std::cout << " day 14 part 2 : " << reg.sum() << "\n";
}