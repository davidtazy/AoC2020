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

using namespace std::literals::string_literals;

struct Schedule {
  int departure;
  std::vector<int> freq;
};

Schedule parse_bus_schedule(std::istream& in) {
  Schedule sched;

  in >> sched.departure;

  while (in.good()) {
    if (std::isdigit(in.peek())) {
      int val;
      in >> val;

      sched.freq.push_back(val);
    }
    in.ignore();
  }

  return sched;
}

#include <limits>
std::tuple<int, int> find_bus_id_and_wait(const Schedule& sched) {
  int bus_id, wait = std::numeric_limits<int>::max();

  for (auto id : sched.freq) {
    int w = (id - sched.departure % id) % id;
    if (w < wait) {
      wait = w;
      bus_id = id;
    }
  }
  return {bus_id, wait};
}

TEST_CASE("Day 13: Shuttle Search") {
  std::string data(R"_(939
7,13,x,x,59,x,31,19)_");
  std::istringstream in{data};

  auto cmds = parse_bus_schedule(in);

  SECTION("part 1") {
    REQUIRE(cmds.departure == 939);
    REQUIRE(cmds.freq.size() == 5);
    auto [bus_id, wait] = find_bus_id_and_wait(cmds);
    REQUIRE(bus_id == 59);
    REQUIRE(wait == 5);
  }
};

TEST_CASE("day 13 part 1 ") {
  std::ifstream in(DATA_DIR "/dataset/input_13.txt", std::ifstream::in);
  REQUIRE(in.good());

  auto cmds = parse_bus_schedule(in);

  auto [bus_id, wait] = find_bus_id_and_wait(cmds);

  std::cout << " day 13 part 1 : " << bus_id * wait << "\n";
}

struct Bus {
  Bus(int freq, int offset) : freq(freq), offset(offset), lcm(0) {
    auto n = freq - offset;
    while (n < 0) {
      n += freq;
    }
    number = n;
  }
  int freq;
  int offset;
  unsigned long long number;
  unsigned long long lcm;

  unsigned long long inc(unsigned long long max) {
    if (max > number) {
      auto dd = lcm;
      if (dd == 0)
        dd = freq;

      auto d = max - number;
      auto mul = d / dd;
      if (d % dd != 0) {
        mul++;
      }

      number += mul * dd;
    }
    return number;
  }
};

std::vector<Bus> parse_bus_schedule2(std::istream& in) {
  std::vector<Bus> sched;

  int ignore;
  in >> ignore;
  in.ignore();
  int offset = 0;
  while (in.good()) {
    if (std::isdigit(in.peek())) {
      int val;
      in >> val;

      sched.emplace_back(Bus{val, offset});
    } else {
      assert(in.peek() == 'x');
      in.ignore();
    }
    in.ignore();
    offset++;
  }

  return sched;
}

unsigned long long find_repeated_cycle_brut_force(std::vector<Bus> numbers) {
  auto max = numbers.front().number;
  auto prec_max = max;
  do {
    prec_max = max;
    ranges::for_each(numbers, [&max](Bus& bus) {
      if (bus.number < max) {
        max = bus.inc(max);
      }
    });

  } while (prec_max != max);

  return max;
}

TEST_CASE("test_algo brut force") {
  SECTION("first example") {
    std::vector<Bus> busses = {{17, 0}, {13, 2}, {19, 3}};

    REQUIRE(find_repeated_cycle_brut_force(busses) == 3417);
  }

  SECTION("second example") {
    std::vector<Bus> busses = {{1789, 0}, {37, 1}, {47, 2}, {1889, 3}};

    REQUIRE(find_repeated_cycle_brut_force(busses) == 1202161486);
  }
}

template <typename T>
unsigned long long find_repeated_cycle(T& numbers) {
  auto max = numbers.front().number;
  auto prec_max = max;
  do {
    prec_max = max;
    ranges::for_each(numbers, [&max](Bus& bus) {
      if (bus.number < max) {
        max = bus.inc(max);
      }
    });

  } while (prec_max != max);

  auto lcm = ranges::accumulate(
      numbers, 1ull, [](unsigned long long mul, const Bus& bus) { return mul * bus.freq; });

  ranges::for_each(numbers, [lcm](Bus& bus) { bus.lcm = lcm; });

  return max;
}

unsigned long long day13part2(std::vector<Bus>& busses) {
  unsigned long long ret = 0;
  for (int i : ranges::view::iota(2, (int)busses.size() + 1)) {
    // std::cout << i << "/" << busses.size() << " ret=" << ret << "\n";
    auto slice = busses | ranges::view::slice(0, i);
    ret = find_repeated_cycle(slice);
  }
  return ret;
}

TEST_CASE("test algo optimized") {
  std::vector<Bus> busses = {{17, 0}, {13, 2}, {19, 3}};

  REQUIRE(day13part2(busses) == 3417);
}

TEST_CASE("day 13 part2 ") {
  std::ifstream in(DATA_DIR "/dataset/input_13.txt", std::ifstream::in);
  REQUIRE(in.good());

  auto busses = parse_bus_schedule2(in);

  std::cout << " day 13 part 2 : " << day13part2(busses) << "\n";
}