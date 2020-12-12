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

using Room = std::vector<std::string>;

Room parse_room(std::istream& in) {
  return ranges::getlines(in) | ranges::to_vector;
}

Room room_from_string(std::string str) {
  std::istringstream in(str);
  return parse_room(in);
}

struct Col {
  int val;
};
struct Row {
  int val;
};
struct Position {
  Position() = default;
  Position(Col col, Row row) : col(col.val), row(row.val) {}

  bool is_null() const { return col == -1 && row == -1; }

  [[nodiscard]] Position Move(Position pos) const {
    return {Col{col + pos.col}, Row{row + pos.row}};
  }

  int col{-1};
  int row{-1};
};

struct GameOfLife {
  GameOfLife(const Room& room_) : room{room_} {}

  enum State { Seat, Floor, Occupied };
  State to_state(char c) const {
    switch (c) {
      case '.':
        return State::Floor;
      case 'L':
        return State::Seat;
      case '#':
        return State::Occupied;
    }
    throw std::runtime_error("to state");
  }

  void for_each(std::function<void(Position)> callback) const {
    for (auto row : ranges::views::iota(0, (int)room.size())) {
      for (auto col : ranges::views::iota(0, (int)room.front().size())) {
        callback(Position{Col{col}, Row{row}});
      }
    }
  }

  std::vector<State> neighbors(Position pos) const {
    int row = pos.row;
    int col = pos.col;
    std::vector<State> ret;
    for (auto r : ranges::views::iota(std::max(0, row - 1), std::min((int)room.size(), row + 2))) {
      for (auto c :
           ranges::views::iota(std::max(0, col - 1), std::min((int)room.front().size(), col + 2))) {
        if (r == row && col == c)
          continue;
        ret.push_back(to_state(room.at(r).at(c)));
      }
    }
    return ret;
  }

  bool is_valid(const Position& pos) const {
    if (pos.col < 0 || pos.row < 0)
      return false;
    if (pos.col >= room.front().size() || pos.row >= room.size())
      return false;
    return true;
  }

  int visible_occupied_seat(Position pos, Position dir) const {
    //
    Position next = pos.Move(dir);
    while (is_valid(next) && !is_free_seat(next)) {
      auto cell = at(next);
      if (is_occupied_seat(next))
        return 1;

      next = next.Move(dir);
    }
    return 0;
  }

  int visible_occupied_seats(Position pos) const {
    int ret{0};

    for (auto dir : {Position{Col{0}, Row{1}}, Position{Col{0}, Row{-1}}, Position{Col{1}, Row{0}},
                     Position{Col{-1}, Row{0}}, Position{Col{1}, Row{1}}, Position{Col{1}, Row{-1}},
                     Position{Col{-1}, Row{1}}, Position{Col{-1}, Row{-1}}}) {
      ret += visible_occupied_seat(pos, dir);
    }

    return ret;
  }

  int count_occupied(Position pos) const { return ranges::count(neighbors(pos), State::Occupied); }

  bool is_free_seat(Position pos) const { return at(pos) == Seat; }
  bool is_occupied_seat(Position pos) const { return at(pos) == Occupied; }
  bool is_floor(Position pos) const { return at(pos) == Floor; }

  void set_free(Position pos) { room[pos.row][pos.col] = 'L'; }
  void set_occupied(Position pos) { room[pos.row][pos.col] = '#'; }

  GameOfLife next_round() const {
    GameOfLife next{room};
    for_each([&next, this](Position pos) {
      //
      auto occ = this->count_occupied(pos);

      if (this->is_free_seat(pos) && occ == 0) {
        next.set_occupied(pos);
      } else if (this->is_occupied_seat(pos) && occ >= 4) {
        next.set_free(pos);
      }
    });

    return next;
  }

  GameOfLife next_round_part2() const {
    GameOfLife next{room};
    for_each([&next, this](Position pos) {
      //
      auto occ = this->visible_occupied_seats(pos);

      if (this->is_free_seat(pos) && occ == 0) {
        next.set_occupied(pos);
      } else if (this->is_occupied_seat(pos) && occ >= 5) {
        next.set_free(pos);
      }
    });

    return next;
  }

  int count_occupied_seats() const {
    int count = 0;
    for_each([&count, this](Position pos) {
      if (this->is_occupied_seat(pos))
        count++;
    });
    return count;
  }

  int converge() const {
    GameOfLife next(room);
    auto pred = next;
    do {
      pred = next;
      next = next.next_round();
      // std::cout << next.count_occupied_seats() << '\n';

    } while (next.room != pred.room);

    return next.count_occupied_seats();
  }

  int converge_part2() const {
    GameOfLife next(room);
    auto pred = next;
    do {
      pred = next;
      next = next.next_round_part2();
      // std::cout << next.count_occupied_seats() << '\n';

    } while (next.room != pred.room);

    return next.count_occupied_seats();
  }

  State at(Position& pos) const { return to_state(room.at(pos.row).at(pos.col)); }

  Room room;
};

TEST_CASE("Day 11: Seating System") {
  std::string data(R"_(L.LL.LL.LL
LLLLLLL.LL
L.L.L..L..
LLLL.LL.LL
L.LL.LL.LL
L.LLLLL.LL
..L.L.....
LLLLLLLLLL
L.LLLLLL.L
L.LLLLL.LL)_");
  std::istringstream in{data};

  auto room = parse_room(in);

  SECTION("part 1") {
    REQUIRE(room.size() == 10);
    REQUIRE(room.front().size() == 10);

    GameOfLife gol{room};

    REQUIRE(gol.neighbors(Position{Col{0}, Row{0}}) ==
            std::vector<GameOfLife::State>{GameOfLife::Floor, GameOfLife::Seat, GameOfLife::Seat});
    REQUIRE(gol.neighbors(Position{Col{9}, Row{9}}) ==
            std::vector<GameOfLife::State>{GameOfLife::Floor, GameOfLife::Seat, GameOfLife::Seat});

    REQUIRE(gol.count_occupied(Position{Col{0}, Row{0}}) == 0);

    // ------ first round -------
    auto next = gol.next_round();
    auto expected = room_from_string(R"_(#.##.##.##
#######.##
#.#.#..#..
####.##.##
#.##.##.##
#.#####.##
..#.#.....
##########
#.######.#
#.#####.##)_");

    REQUIRE(next.room == expected);

    // ------ second round -------

    next = next.next_round();

    expected = room_from_string(R"_(#.LL.L#.##
#LLLLLL.L#
L.L.L..L..
#LLL.LL.L#
#.LL.LL.LL
#.LLLL#.##
..L.L.....
#LLLLLLLL#
#.LLLLLL.L
#.#LLLL.##)_");

    REQUIRE(next.room == expected);

    // ------ third round -------

    next = next.next_round();

    expected = room_from_string(R"_(#.##.L#.##
#L###LL.L#
L.#.#..#..
#L##.##.L#
#.##.LL.LL
#.###L#.##
..#.#.....
#L######L#
#.LL###L.L
#.#L###.##)_");

    REQUIRE(next.room == expected);

    // ------ fourth round -------

    next = next.next_round();

    expected = room_from_string(R"_(#.#L.L#.##
#LLL#LL.L#
L.L.L..#..
#LLL.##.L#
#.LL.LL.LL
#.LL#L#.##
..L.L.....
#L#LLLL#L#
#.LLLLLL.L
#.#L#L#.##)_");

    REQUIRE(next.room == expected);

    // ------ fifth  round -------

    next = next.next_round();

    expected = room_from_string(R"_(#.#L.L#.##
#LLL#LL.L#
L.#.L..#..
#L##.##.L#
#.#L.LL.LL
#.#L#L#.##
..L.L.....
#L#L##L#L#
#.LLLLLL.L
#.#L#L#.##)_");

    REQUIRE(next.room == expected);

    // --- no change
    next = next.next_round();
    REQUIRE(next.room == expected);

    // ----
    REQUIRE(gol.converge() == 37);

    // auto [number_of_one, number_of_three] = diff_and_find_number_of_1_and_3(numbers);

    // REQUIRE(number_of_one == 7);
    // REQUIRE(number_of_three == 5);
  }

  SECTION("part 2") {
    GameOfLife gol{room};

    REQUIRE(gol.converge_part2() == 26);
  };
};

TEST_CASE("day 11  ") {
  std::ifstream in(DATA_DIR "/dataset/input_11.txt", std::ifstream::in);
  REQUIRE(in.good());

  auto room = parse_room(in);
  GameOfLife gol{room};

  std::cout << " day 11 part 1 : " << gol.converge() << "\n";

  std::cout << " day 11 part 2 : " << gol.converge_part2() << "\n";
}

TEST_CASE("visible_occupied_seats") {
  auto room = room_from_string(R"_(.............
.L.L.#.#.#.#.
.............)_");
  GameOfLife gol(room);

  REQUIRE(gol.visible_occupied_seats(Position{Col{1}, Row{1}}) == 0);
  REQUIRE(gol.visible_occupied_seats(Position{Col{3}, Row{1}}) == 1);
}
