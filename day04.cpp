#include <array>
#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <vector>

std::vector<std::string> split(std::istream& in, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;

  while (std::getline(in, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
  std::istringstream in(str);
  return split(in, delimiter);
}

bool has_four_digits(const std::string& str) {
  if (str.size() != 4) {
    return false;
  }

  return std::all_of(std::begin(str), std::end(str), ::isdigit);
}

bool between(std::string str, int min, int max) try {
  int number = std::stoi(str);

  return min <= number && number <= max;

} catch (std::exception e) {
  return false;
}

struct Document {
  void add_info(std::string key, std::string value) { dico[key] = value; }

  int info_count() const { return dico.size(); }

  bool contains(const std::string& info) const { return dico.find(info) != dico.end(); }

  bool has_required_fields() const {
    std::vector<std::string> expected_infos{"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"};

    return std::all_of(std::begin(expected_infos), std::end(expected_infos),
                       [this](const std::string& info) { return this->contains(info); });
  };

  bool is_year_valid(const std::string str, int min, int max) const {
    auto val = dico.at(str);
    if (!has_four_digits(val))
      return false;

    return between(val, min, max);
  }

  bool is_birthyear_valid() const { return is_year_valid("byr", 1920, 2002); }
  bool is_issueyear_valid() const { return is_year_valid("iyr", 2010, 2020); }
  bool is_expiration_year_valid() const { return is_year_valid("eyr", 2020, 2030); }

  bool is_height_valid() const {
    auto hgt = dico.at("hgt");
    std::regex pattern("[0-9]+(cm|in)");
    if (!std::regex_match(hgt, pattern)) {
      return false;
    }

    if (hgt.find("cm") != std::string::npos) {
      return between(hgt, 150, 193);
    } else {
      return between(hgt, 59, 76);
    }
  }

  bool is_hair_color_valid() const {
    auto hcl = dico.at("hcl");

    if (hcl.size() != 7)
      return false;
    std::regex pattern("#[0-9a-f]+");
    return std::regex_match(hcl, pattern);
  }

  bool is_passport_id_valid() const {
    auto pid = dico.at("pid");

    if (pid.size() != 9)
      return false;
    std::regex pattern("[0-9]+");
    return std::regex_match(pid, pattern);
  }

  bool is_eyecolor_valid() const {
    auto ecl = dico.at("ecl");

    std::regex pattern("amb|blu|brn|gry|grn|hzl|oth");
    return std::regex_match(ecl, pattern);
  }

  bool is_valid() const {
    return has_required_fields() && is_birthyear_valid() && is_issueyear_valid() &&
           is_expiration_year_valid() && is_height_valid() && is_hair_color_valid() &&
           is_passport_id_valid() && is_eyecolor_valid();
  }

 private:
  std::map<std::string, std::string> dico;
};

using Batch = std::vector<Document>;

Document read_document(std::istream& in) {
  Document doc;
  for (std::string line; std::getline(in, line);) {
    if (line.empty()) {
      break;
    }
    auto elements = split(line, ' ');

    for (auto element : elements) {
      auto info = split(element, ':');
      doc.add_info(info.at(0), info.at(1));
    }
  }

  return doc;
}

Batch read_batch(std::istream& in) {
  Batch batch;

  while (in.good()) {
    batch.push_back(read_document(in));
  }

  return batch;
}

int count_required_fields_passports(const Batch& batch) {
  return std::count_if(std::begin(batch), std::end(batch),
                       [](const Document& doc) { return doc.has_required_fields(); });
}

int count_valid_passports(const Batch& batch) {
  return std::count_if(std::begin(batch), std::end(batch),
                       [](const Document& doc) { return doc.is_valid(); });
}

TEST_CASE("Passport processing example") {
  std::stringstream in(R"_(ecl:gry pid:860033327 eyr:2020 hcl:#fffffd
byr:1937 iyr:2017 cid:147 hgt:183cm

iyr:2013 ecl:amb cid:350 eyr:2023 pid:028048884
hcl:#cfa07d byr:1929

hcl:#ae17e1 iyr:2013
eyr:2024
ecl:brn pid:760753108 byr:1931
hgt:179cm

hcl:#cfa07d eyr:2025 pid:166559648
iyr:2011 ecl:brn hgt:59in)_");

  auto batch = read_batch(in);

  REQUIRE(batch.size() == 4);

  REQUIRE(batch.front().info_count() == 8);
  REQUIRE(batch.back().info_count() == 6);

  REQUIRE(count_required_fields_passports(batch) == 2);
}

TEST_CASE("id validation") {
  REQUIRE(has_four_digits("1234") == true);
  REQUIRE(has_four_digits("123") == false);
  REQUIRE(has_four_digits("12345") == false);
  REQUIRE(has_four_digits("123a") == false);

  Document doc;

  doc.add_info("iyr", "2015");
  doc.add_info("eyr", "2025");

  doc.add_info("byr", "1979");
  REQUIRE(doc.is_birthyear_valid() == true);

  doc.add_info("byr", "2002");
  REQUIRE(doc.is_birthyear_valid() == true);

  doc.add_info("byr", "2003");
  REQUIRE(doc.is_birthyear_valid() == false);

  REQUIRE(doc.is_issueyear_valid() == true);

  doc.add_info("hgt", "170cm");
  REQUIRE(doc.is_height_valid() == true);

  doc.add_info("hgt", "65in");
  REQUIRE(doc.is_height_valid() == true);

  doc.add_info("hgt", "175in");
  REQUIRE(doc.is_height_valid() == false);

  doc.add_info("hgt", "175");
  REQUIRE(doc.is_height_valid() == false);

  doc.add_info("hcl", "#1234a5");
  REQUIRE(doc.is_hair_color_valid() == true);

  doc.add_info("hcl", "#1234a");
  REQUIRE(doc.is_hair_color_valid() == false);
  doc.add_info("hcl", "#123r45");
  REQUIRE(doc.is_hair_color_valid() == false);

  doc.add_info("hcl", "123abc");
  REQUIRE(doc.is_hair_color_valid() == false);

  doc.add_info("pid", "000000001");
  REQUIRE(doc.is_passport_id_valid() == true);

  doc.add_info("pid", "0123456789");
  REQUIRE(doc.is_passport_id_valid() == false);

  doc.add_info("ecl", "sdf");
  REQUIRE(doc.is_eyecolor_valid() == false);
  doc.add_info("ecl", "gry");
  REQUIRE(doc.is_eyecolor_valid() == true);

  doc.add_info("ecl", "wat");
  REQUIRE(doc.is_eyecolor_valid() == false);
}

TEST_CASE("part2 valid example") {
  std::istringstream in(
      R"_(iyr:2010 hgt:158cm hcl:#b6652a ecl:blu byr:1944 eyr:2021 pid:093154719)_");

  auto batch = read_batch(in);

  REQUIRE(count_valid_passports(batch) == 1);
}

TEST_CASE("part2 example") {
  std::istringstream in(R"_(eyr:1972 cid:100
hcl:#18171d ecl:amb hgt:170 pid:186cm iyr:2018 byr:1926

iyr:2019
hcl:#602927 eyr:1967 hgt:170cm
ecl:grn pid:012533040 byr:1946

hcl:dab227 iyr:2012
ecl:brn hgt:182cm pid:021572410 eyr:2020 byr:1992 cid:277

hgt:59cm ecl:zzz
eyr:2038 hcl:74454a iyr:2023
pid:3556412378 byr:2007

pid:087499704 hgt:74in ecl:grn iyr:2012 eyr:2030 byr:1980
hcl:#623a2f

eyr:2029 ecl:blu cid:129 byr:1989
iyr:2014 pid:896056539 hcl:#a97842 hgt:165cm

hcl:#888785
hgt:164cm byr:2001 iyr:2015 cid:88
pid:545766238 ecl:hzl
eyr:2022

iyr:2010 hgt:158cm hcl:#b6652a ecl:blu byr:1944 eyr:2021 pid:093154719)_");

  auto batch = read_batch(in);

  REQUIRE(count_valid_passports(batch) == 4);
}

TEST_CASE("day 4  ") {
  std::ifstream in(DATA_DIR "/dataset/input_04.txt", std::ifstream::in);
  REQUIRE(in.good());

  auto batch = read_batch(in);

  std::cout << " day 4 part 1 : " << count_required_fields_passports(batch) << "\n";
  std::cout << " day 4 part 2 : " << count_valid_passports(batch) << "\n";
}