
#include <array>
#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

struct PasswordLine {
  int min;
  int max;
  char ch;
  std::string password;

  bool operator==(const PasswordLine& other) const { return to_tuple() == other.to_tuple(); }

 private:
  std::tuple<int, int, char, std::string> to_tuple() const {
    return std::make_tuple(min, max, ch, password);
  }
};

std::vector<PasswordLine> read_password_table(std::istream& in) {
  std::vector<PasswordLine> table;
  while (in.good()) {
    PasswordLine row;
    in >> row.min;
    in.ignore();
    in >> row.max;
    in >> row.ch;
    in.ignore();
    in >> row.password;
    table.push_back(row);
  }

  return table;
}

bool is_valid(const PasswordLine& row) {
  int nb_occurrence = std::count(row.password.begin(), row.password.end(), row.ch);

  return row.min <= nb_occurrence && nb_occurrence <= row.max;
}

bool is_valid_new_policy(const PasswordLine& row) {
  return row.password.at(row.min - 1) == row.ch ^ row.password.at(row.max - 1) == row.ch;
}

int count_valid_password(const std::vector<PasswordLine>& table,
                         std::function<int(const PasswordLine&)> predicate) {
  return std::count_if(table.begin(), table.end(),
                       [&predicate](const PasswordLine& row) { return predicate(row); });
}

TEST_CASE("day 2 example") {
  std::stringstream in(R"_(1-3 a: abcde
1-3 b: cdefg
2-9 c: ccccccccc)_");

  SECTION("read password table") {
    auto table = read_password_table(in);
    REQUIRE(table.size() == 3);
    REQUIRE(table.front() == PasswordLine{1, 3, 'a', "abcde"});
  }

  SECTION("valid and count part 1") {
    auto table = read_password_table(in);

    REQUIRE(is_valid(table.at(0)) == true);
    REQUIRE(is_valid(table.at(1)) == false);
    REQUIRE(is_valid(table.at(2)) == true);

    REQUIRE(count_valid_password(table, is_valid) == 2);
  }

  SECTION("valid and coun part 2") {
    auto table = read_password_table(in);

    REQUIRE(is_valid_new_policy(table.at(0)) == true);
    REQUIRE(is_valid_new_policy(table.at(1)) == false);
    REQUIRE(is_valid_new_policy(table.at(2)) == false);

    REQUIRE(count_valid_password(table, is_valid_new_policy) == 1);
  }
}

TEST_CASE("day 2  ") {
  std::ifstream in(DATA_DIR "/dataset/input_02.txt", std::ifstream::in);
  REQUIRE(in.good());

  auto table = read_password_table(in);

  std::cout << " day 2 part 1 : " << count_valid_password(table, is_valid) << "\n";
  std::cout << " day 2 part 2 : " << count_valid_password(table, is_valid_new_policy) << "\n";
}
