#include <array>
#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using BinaryBoardingPasses = std::vector<std::string>;

BinaryBoardingPasses read_binary_boarding_passes(std::istream& in) {
  BinaryBoardingPasses bp;
  while (in.good()) {
    std::string line;
    in >> line;
    bp.push_back(line);
  }
  return bp;
}

struct Range {
  int min;
  int max;

  Range lower_half() const { return {min, min + ((max - min + 1) / 2) - 1}; }

  Range upper_half() const { return {min + ((max - min + 1) / 2), max}; }

  bool operator==(const Range& other) const {
    return std::tuple(min, max) == std::tuple(other.min, other.max);
  }
};

struct BinarySeat {
  Range row{0, 127};
  Range col{0, 7};

  BinarySeat() = default;
  BinarySeat(int row_p, int col_p) : row{row_p, row_p}, col{col_p, col_p} {}

  int id() const { return row.min * 8 + col.min; }

  bool operator==(const BinarySeat& other) const {
    return std::tuple(row, col) == std::tuple(other.row, other.col);
  }

  bool operator<(const BinarySeat& other) const { return id() < other.id(); }
};

std::ostream& operator<<(std::ostream& out, const Range& seat) {
  return out << '[' << seat.min << ',' << seat.max << ']';
}

std::ostream& operator<<(std::ostream& out, const BinarySeat& seat) {
  return out << '[' << seat.row << ',' << seat.col << ']';
}
BinarySeat to_seat(std::string boarding_pass) {
  BinarySeat s;
  for (char c : boarding_pass) {
    if (c == 'F')
      s.row = s.row.lower_half();
    if (c == 'B')
      s.row = s.row.upper_half();
    if (c == 'L')
      s.col = s.col.lower_half();
    if (c == 'R')
      s.col = s.col.upper_half();
  }
  return s;
}

using BoardingPasses = std::vector<BinarySeat>;

BoardingPasses to_boarding_passes(const BinaryBoardingPasses& bbp) {
  BoardingPasses bp;
  for (std::string pass : bbp) {
    bp.push_back(to_seat(pass));
  }
  return bp;
}

int highest_seat_id(const BinaryBoardingPasses& bbp) {
  BoardingPasses bp = to_boarding_passes(bbp);

  return std::max_element(std::begin(bp), std::end(bp))->id();
}

TEST_CASE("Binary Boarding example") {
  std::stringstream in(R"_(BFFFBBFRRR
FFFBBBFRRR
BBFFBBFRLL)_");

  auto bp = read_binary_boarding_passes(in);
  REQUIRE(bp.size() == 3);
  REQUIRE(bp.front() == "BFFFBBFRRR");

  REQUIRE(Range{0, 127}.lower_half() == Range{0, 63});
  REQUIRE(Range{0, 63}.upper_half() == Range{32, 63});
  REQUIRE(Range{32, 63}.lower_half() == Range{32, 47});
  REQUIRE(Range{32, 47}.upper_half() == Range{40, 47});
  REQUIRE(Range{40, 47}.upper_half() == Range{44, 47});
  REQUIRE(Range{44, 47}.lower_half() == Range{44, 45});
  REQUIRE(Range{44, 45}.upper_half() == Range{45, 45});
  REQUIRE(Range{44, 45}.lower_half() == Range{44, 44});

  REQUIRE(Range{0, 127}.upper_half() == Range{64, 127});

  REQUIRE(to_seat(bp.at(0)) == BinarySeat{70, 7});
  REQUIRE(to_seat(bp.at(1)) == BinarySeat{14, 7});
  REQUIRE(to_seat(bp.at(2)) == BinarySeat{102, 4});

  REQUIRE(BinarySeat{102, 4}.id() == 820);

  REQUIRE(highest_seat_id(bp) == 820);
}

struct Seat {
  int id{-1};
  bool taken{false};
  int col{-1};
  int row{-1};
};

struct Plane {
  Plane() = delete;

  Plane(int row, int col) {
    for (int i_row = 0; i_row < row; i_row++) {
      for (int i_col = 0; i_col < col; i_col++) {
        seats.push_back(Seat{i_row * 8 + i_col, false, i_col, i_row});
      }
    }
  }

  void take(const BinarySeat& seat) {
    int col = seat.col.min;
    int row = seat.row.min;
    auto& s = at(col, row);
    s.id = seat.id();
    s.taken = true;
    s.col = col;
    s.row = row;
  }

  void for_each(std::function<void(int, int, const Seat&)> callback) const {
    int row_max = seats.back().row + 1;
    int col_max = seats.back().col + 1;

    for (int row = 0; row < row_max; row++) {
      for (int col = 0; col < col_max; col++) {
        callback(col, row, at(col, row));
      }
    }
  }

  const Seat& seat_with_id(int id) const {
    return *std::find_if(std::begin(seats), std::end(seats),
                         [id](const Seat& seat) { return seat.id == id; });
  }

  std::vector<Seat> free_seats() const {
    std::vector<Seat> vseats;

    std::copy_if(std::begin(seats), std::end(seats), std::back_inserter(vseats),
                 [](const Seat& seat) { return seat.taken == false; });

    return vseats;
  }
  Seat find_first_free_seat_with_neigbors() const {
    auto f_seats = free_seats();

    return *std::find_if(std::begin(f_seats), std::end(f_seats), [this](const Seat& seat) {
      int id = seat.id;
      return seat.taken == false && (this->seat_with_id(id - 1).taken == true) &&
             (this->seat_with_id(id + 1).taken == true);
    });
  }

  Seat& at(int col, int row) {
    auto it = std::find_if(std::begin(seats), std::end(seats), [col, row](const Seat& seat) {
      return seat.col == col && seat.row == row;
    });

    return *it;
  }
  const Seat& at(int col, int row) const {
    auto it = std::find_if(std::begin(seats), std::end(seats), [col, row](const Seat& seat) {
      return seat.col == col && seat.row == row;
    });

    return *it;
  }

 private:
  std::vector<Seat> seats;
};

std::ostream& operator<<(std::ostream& out, const Plane& plane) {
  plane.for_each([&out](int col, int row, const Seat& seat) {
    if (col == 0)
      out << '\n';
    out << seat.taken << ' ';
  });
  return out;
}

TEST_CASE(" plane") {
  Plane p(3, 3);

  p.take(BinarySeat{1, 0});
  p.take(BinarySeat{1, 2});

  REQUIRE(p.free_seats().size() == 7);
  REQUIRE(p.seat_with_id(9).id == 9);

  auto my_seat = p.find_first_free_seat_with_neigbors();

  INFO(p);

  REQUIRE(my_seat.col == 1);
  REQUIRE(my_seat.row == 1);

  // std::cout << p << '\n';
}

TEST_CASE("day 5  ") {
  std::ifstream in(DATA_DIR "/dataset/input_05.txt", std::ifstream::in);
  REQUIRE(in.good());

  auto bbp = read_binary_boarding_passes(in);
  auto bp = to_boarding_passes(bbp);

  Plane p(128, 8);
  for (const auto& pass : bp) {
    p.take(pass);
  }

  std::cout << " day 5 part 1 : " << highest_seat_id(bbp) << "\n";
  std::cout << " day 5 part 2 : " << p.find_first_free_seat_with_neigbors().id << "\n";
}