#include <array>
#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <range/v3/all.hpp>  // get everything
#include <set>
#include <sstream>
#include <vector>

struct Bag {
  std::string color;

  bool contains(const std::string& color) const { return ranges::contains(bags, color); }

  std::vector<std::string> bags;

  std::vector<int> count;
};

struct Bags {
  std::vector<std::string> keys() const {
    return ranges::views::all(dico) | ranges::views::keys | ranges::to_vector;
  }

  bool contains(std::string color) const { return dico.find(color) != std::end(dico); }

  Bag bag(const std::string& color) const { return dico.at(color); }

  std::map<std::string, Bag> dico;
};

Bags parse_bags(std::istream& in) {
  using namespace std::literals::string_literals;
  auto bags =
      ranges::getlines_view(in) |  // line by line
      ranges::views::transform([](std::string line) {
        // std::cout << line << "//";
        auto color_bags =
            line | ranges::view::split(" bags contain"s) |
            ranges::views::transform([](auto line) { return line | ranges::to<std::string>; }) |
            ranges::to_vector;  //

        // std::cout << ranges::views::all(color_bags) << "//\n";

        std::string color = color_bags.at(0);

        std::string bags_line = color_bags.at(1);
        // std::cout << bags_line << "//\n";

        // return line;
        auto bags_vector = bags_line | ranges::view::split(","s) |
                           ranges::views::transform(
                               [](auto raw_bag) { return raw_bag | ranges::to<std::string>; }) |
                           ranges::views::transform([](std::string raw_bag) {
                             int ignore_number;
                             std::string col, lor;
                             std::istringstream in(raw_bag);
                             in >> ignore_number >> col >> lor;
                             return col + " " + lor;
                           }) |
                           ranges::to_vector;

        auto bags_count_vector = bags_line | ranges::view::split(","s) |
                                 ranges::views::transform([](auto raw_bag) {
                                   return raw_bag | ranges::to<std::string>;
                                 }) |
                                 ranges::views::transform([](std::string raw_bag) {
                                   if (raw_bag == "no other_bags") {
                                     return 0;
                                   }
                                   int number;
                                   std::istringstream in(raw_bag);
                                   in >> number;
                                   return number;
                                 }) |
                                 ranges::to_vector;

        return Bag{color, bags_vector, bags_count_vector};
      }) |
      ranges::to_vector;

  std::map<std::string, Bag> dico;

  for (auto& bag : bags) {
    dico[bag.color] = bag;
  }

  return {dico};
}

bool contains(const std::string color, Bag in_bag, const Bags& dico) {
  if (in_bag.contains(color))
    return true;

  return ranges::any_of(in_bag.bags, [color, &dico](std::string bag_color) {
    if (!dico.contains(bag_color))
      return false;
    return contains(color, dico.bag(bag_color), dico);
  });
}

int count_potential_containing(const std::string bag_color, Bags dico) {
  auto keys = dico.keys();

  return ranges::accumulate(keys, 0, [bag_color, &dico](int accu, std::string test_bag_color) {
    if (dico.contains(test_bag_color) && contains(bag_color, dico.bag(test_bag_color), dico)) {
      accu++;
    }
    return accu;
  });
}

int count_bags(const Bag& current, Bags dico) {
  int sum = 0;

  for (auto [i, bag] : current.bags | ranges::views::enumerate) {
    if (dico.contains(bag)) {
      sum += (current.count.at(i)) + (current.count.at(i) * count_bags(dico.bag(bag), dico));
    }
  }

  // std::cout << current.color << " contains " << sum << "\n";

  return sum;
}

TEST_CASE("Day 7: Handy Haversackss example") {
  SECTION("test count_potential_containing") {
    Bag red{"red", {"blue", "yellow"}};
    Bag green{"green", {"blue", "yellow"}};
    Bag brown{"brown", {"purple", "grey"}};

    Bags dico{{{"red", red}, {"green", green}, {"brown", brown}}};

    REQUIRE(contains("blue", red, dico) == true);
    REQUIRE(contains("blue", green, dico) == true);
    REQUIRE(contains("blue", brown, dico) == false);

    REQUIRE(count_potential_containing("blue", dico) == 2);
  };

  SECTION("test part 1 example") {
    std::string data(R"_(light red bags contain 1 bright white bag, 2 muted yellow bags.
dark orange bags contain 3 bright white bags, 4 muted yellow bags.
bright white bags contain 1 shiny gold bag.
muted yellow bags contain 2 shiny gold bags, 9 faded blue bags.
shiny gold bags contain 1 dark olive bag, 2 vibrant plum bags.
dark olive bags contain 3 faded blue bags, 4 dotted black bags.
vibrant plum bags contain 5 faded blue bags, 6 dotted black bags.
faded blue bags contain no other bags.
dotted black bags contain no other bags.)_");
    std::istringstream in1{data};

    auto dico = parse_bags(in1);

    REQUIRE(dico.contains("light red"));
    REQUIRE(dico.keys() == std::vector<std::string>{"bright white", "dark olive", "dark orange",
                                                    "dotted black", "faded blue", "light red",
                                                    "muted yellow", "shiny gold", "vibrant plum"});

    REQUIRE(count_potential_containing("shiny gold", dico) == 4);
    REQUIRE(count_bags(dico.bag("shiny gold"), dico) == 32);
  };

  SECTION("test part 2 example") {
    std::string data(R"_(shiny gold bags contain 2 dark red bags.
dark red bags contain 2 dark orange bags.
dark orange bags contain 2 dark yellow bags.
dark yellow bags contain 2 dark green bags.
dark green bags contain 2 dark blue bags.
dark blue bags contain 2 dark violet bags.
dark violet bags contain no other bags.)_");
    std::istringstream in1{data};

    auto dico = parse_bags(in1);

    REQUIRE(count_bags(dico.bag("shiny gold"), dico) == 126);
  };
}

TEST_CASE("day 7  ") {
  std::ifstream in(DATA_DIR "/dataset/input_07.txt", std::ifstream::in);
  REQUIRE(in.good());
  auto dico = parse_bags(in);

  std::cout << " day 7 part 1 : " << count_potential_containing("shiny gold", dico) << "\n";
  std::cout << " day 7 part 2 : " << count_bags(dico.bag("shiny gold"), dico) << "\n";
}