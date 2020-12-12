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

struct Easting {
  explicit Easting(int v) : v(v) {}
  int v;
};
struct Northing {
  explicit Northing(int v) : v(v) {}
  int v;
};
struct Heading {
  explicit Heading(int v) : v(v) {}
  int v;
};

struct Pos {
  Pos(Easting e, Northing n = Northing{0}, Heading h = Heading{0})
      : easting(e.v), northing(n.v), heading(h.v) {}

  Pos(Northing n) : northing(n.v) {}

  int northing{};
  int easting{};
  int heading{};

  Pos Move(Pos pos) const {
    Pos p(*this);
    p.northing += pos.northing;
    p.easting += pos.easting;
    return p;
  }

  Pos Turn(int angle) const {
    Pos ret(*this);
    ret.heading += angle;
    if (ret.heading > 360)
      ret.heading -= 360;
    if (ret.heading < 0)
      ret.heading += 360;
    if (ret.heading == 360)
      ret.heading = 0;
    return ret;
  }

  Pos Forward(int speed) const {
    switch (heading) {
      case 0:
        return Move(Pos{Northing{speed}});
      case 180:
        return Move(Pos{Northing{-speed}});
      case 90:
        return Move(Pos{Easting{speed}});
      case 270:
        return Move(Pos{Easting{-speed}});
    }
    using namespace std::literals::string_literals;
    throw std::runtime_error("forward heading is "s + std::to_string(heading));
  }

  Pos rotate(int angle) const {
    Pos p(*this);  //
    switch (angle) {
      case 180:
      case -180:
        return Pos{Easting{-easting}, Northing{-northing}};

      case 90:
      case -270:
        return Pos{Easting{northing}, Northing{-easting}};

      case -90:
      case 270:
        return Pos{Easting{-northing}, Northing{easting}};
      default:

        break;
    }
    using namespace std::literals::string_literals;
    throw std::runtime_error("rotate "s + std::to_string(angle));
  }

  bool operator==(const Pos& other) const {
    auto to = [](const Pos& p) -> std::tuple<int, int, int> {
      return std::tuple(p.easting, p.northing, p.heading);
    };
    return to(*this) == to(other);
  }

  Pos scale(int scale) { return {Easting{scale * easting}, Northing{scale * northing}}; }
};
std::ostream& operator<<(std::ostream& out, const Pos& pos) {
  return out << 'N' << pos.northing << 'E' << pos.easting << 'H' << pos.heading;
}

struct Cmd {
  char cmd;
  int val;
};
using Commands = std::vector<Cmd>;

Commands parse_pilot_commands(std::istream& in) {
  Commands cmds;
  while (in.good()) {
    Cmd cmd;
    in >> cmd.cmd >> cmd.val;
    cmds.push_back(cmd);
  }

  return cmds;
}

Pos execute(Cmd cmd, Pos pos) {
  switch (cmd.cmd) {
    case 'F':
      return pos.Forward(cmd.val);
    case 'N':
      return pos.Move(Pos{Northing{cmd.val}});
    case 'S':
      return pos.Move(Pos{Northing{-cmd.val}});
    case 'E':
      return pos.Move(Pos{Easting{cmd.val}});
    case 'W':
      return pos.Move(Pos{Easting{-cmd.val}});
    case 'L':
      return pos.Turn(-cmd.val);
    case 'R':
      return pos.Turn(cmd.val);

    default:
      throw std::runtime_error("execute");
  }
}

std::tuple<Pos, Pos> execute_part2(Cmd cmd, Pos pos, Pos waypoint) {
  switch (cmd.cmd) {
    case 'F': {
      auto dest = waypoint.scale(cmd.val);
      return {pos.Move(dest), waypoint};
    }
    case 'N':
      return {pos, waypoint.Move(Pos{Northing{cmd.val}})};
    case 'S':
      return {pos, waypoint.Move(Northing{-cmd.val})};
    case 'E':
      return {pos, waypoint.Move(Easting{cmd.val})};
    case 'W':
      return {pos, waypoint.Move(Easting{-cmd.val})};
    case 'L':
      return {pos, waypoint.rotate(-cmd.val)};
    case 'R':
      return {pos, waypoint.rotate(cmd.val)};

    default:
      throw std::runtime_error("execute part2");
  }
}

int manhattan(const Pos& pos) {
  return std::abs(pos.easting) + std::abs(pos.northing);
}

TEST_CASE("Day 12: Rain Risk") {
  std::string data(R"_(F10
N3
F7
R90
F11)_");
  std::istringstream in{data};

  auto cmds = parse_pilot_commands(in);

  SECTION("part 1") {
    REQUIRE(cmds.size() == 5);

    Pos pos(Easting{0}, Northing{0}, Heading{90});

    pos = execute(cmds.at(0), pos);
    REQUIRE(pos == Pos{Easting{10}, Northing{0}, Heading{90}});

    pos = execute(cmds.at(1), pos);
    REQUIRE(pos == Pos{Easting{10}, Northing{3}, Heading{90}});

    pos = execute(cmds.at(2), pos);
    REQUIRE(pos == Pos{Easting{17}, Northing{3}, Heading{90}});

    pos = execute(cmds.at(3), pos);
    REQUIRE(pos == Pos{Easting{17}, Northing{3}, Heading{180}});

    pos = execute(cmds.at(4), pos);
    REQUIRE(pos == Pos{Easting{17}, Northing{-8}, Heading{180}});

    REQUIRE(manhattan(pos) == 25);
  }

  SECTION("part 2") {
    Pos pos(Easting{0}, Northing{0}, Heading{90});
    Pos wpt(Easting{10}, Northing{1});

    std::tie(pos, wpt) = execute_part2(cmds.at(0), pos, wpt);
    REQUIRE(pos == Pos{Easting{100}, Northing{10}, Heading{90}});
    REQUIRE(wpt == Pos{Easting{10}, Northing{1}});

    std::tie(pos, wpt) = execute_part2(cmds.at(1), pos, wpt);
    REQUIRE(pos == Pos{Easting{100}, Northing{10}, Heading{90}});
    REQUIRE(wpt == Pos{Easting{10}, Northing{4}});

    std::tie(pos, wpt) = execute_part2(cmds.at(2), pos, wpt);
    REQUIRE(pos == Pos{Easting{170}, Northing{38}, Heading{90}});
    REQUIRE(wpt == Pos{Easting{10}, Northing{4}});

    std::tie(pos, wpt) = execute_part2(cmds.at(3), pos, wpt);
    REQUIRE(pos == Pos{Easting{170}, Northing{38}, Heading{90}});
    REQUIRE(wpt == Pos{Easting{4}, Northing{-10}});

    std::tie(pos, wpt) = execute_part2(cmds.at(4), pos, wpt);
    REQUIRE(pos == Pos{Easting{214}, Northing{-72}, Heading{90}});
    REQUIRE(wpt == Pos{Easting{4}, Northing{-10}});

    REQUIRE(manhattan(pos) == 286);
  }
};

TEST_CASE("day 12 ") {
  std::ifstream in(DATA_DIR "/dataset/input_12.txt", std::ifstream::in);
  REQUIRE(in.good());

  auto cmds = parse_pilot_commands(in);

  SECTION("part 1") {
    Pos pos(Easting{0}, Northing{0}, Heading{90});
    for (auto& cmd : cmds) {
      pos = execute(cmd, pos);
    }

    std::cout << " day 12 part 1 : " << manhattan(pos) << "\n";
  }
  SECTION("part 2") {
    Pos pos(Easting{0}, Northing{0}, Heading{90});
    Pos wpt(Easting{10}, Northing{1});
    for (auto& cmd : cmds) {
      std::tie(pos, wpt) = execute_part2(cmd, pos, wpt);
    }
    std::cout << " day 12 part 2 : " << manhattan(pos) << "\n";
  }
}
