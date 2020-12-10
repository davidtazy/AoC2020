#include <array>
#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <range/v3/all.hpp>  // get everything
#include <set>
#include <sstream>
#include <variant>
#include <vector>

struct Nop {
  int value;
};
struct Acc {
  int value;
};
struct Jmp {
  int offset;
};

using Command = std::variant<Nop, Acc, Jmp>;
using Program = std::vector<Command>;

int command_offset(const Command& cmd) {
  if (std::holds_alternative<Jmp>(cmd)) {
    return std::get<Jmp>(cmd).offset;
  }
  return 1;
}

int acc_offset(const Command& cmd) {
  if (std::holds_alternative<Acc>(cmd)) {
    return std::get<Acc>(cmd).value;
  }
  return 0;
}

struct Cpu {
  explicit Cpu(const Program& pr) : program(pr) {}

  bool finished() const { return pointer == program.size(); }

  void execute() {
    pointer = 0;
    acc = 0;

    while (1) {
      // fin de program
      if (pointer == program.size()) {
        return;
      }

      auto& cmd = program.at(pointer);
      // fin de prog quand ligne deja executee
      if (auto [it, success] = executed_lines.insert(pointer); success == false) {
        return;
      }
      acc += acc_offset(cmd);

      pointer += command_offset(cmd);
    }
  }

  int next_command_to_mutate(int pointer) const {
    auto it =
        std::find_if(std::begin(program) + pointer, std::end(program), [](const Command& cmd) {
          return std::holds_alternative<Jmp>(cmd) || std::holds_alternative<Nop>(cmd);
        });

    return ranges::distance(std::begin(program), it);
  }

  Cpu mutate(int& mutated_pointer) {
    Program prg = program;

    mutated_pointer = next_command_to_mutate(mutated_pointer);

    Command& cmd = prg[mutated_pointer];

    if (std::holds_alternative<Jmp>(cmd)) {
      int v = std::get<Jmp>(cmd).offset;
      Command m{Nop{v}};
      cmd.swap(m);

    } else {
      int v = std::get<Nop>(cmd).value;
      Command m{Jmp{v}};
      cmd.swap(m);
    }

    return Cpu{prg};
  }

  Program program;
  int pointer{0};
  int acc{0};
  std::set<int> executed_lines;
};

Program fix_program(const Program& program) {
  int ptr = 0;

  Cpu cpu{program};
  Cpu mutate{program};
  do {
    mutate = cpu.mutate(ptr);
    ptr++;
    mutate.execute();

  } while (!mutate.finished());

  return mutate.program;
}

Program parse_commands(std::istream& in) {
  Program prog;
  while (in.good()) {
    std::string cmd;
    int value;
    in >> cmd >> value;

    if (cmd == "nop") {
      prog.emplace_back(Nop{value});
    } else if (cmd == "acc") {
      prog.emplace_back(Acc{value});
    } else if (cmd == "jmp") {
      prog.emplace_back(Jmp{value});
    }
  }

  return prog;
}

TEST_CASE("Day 8: Handheld Halting") {
  std::string data(R"_(nop +0
acc +1
jmp +4
acc +3
jmp -3
acc -99
acc +1
jmp -4
acc +6)_");
  std::istringstream in{data};

  auto prog = parse_commands(in);

  SECTION("part 1") {
    REQUIRE(prog.size() == 9);

    REQUIRE(std::holds_alternative<Nop>(prog.front()));
    REQUIRE(std::holds_alternative<Acc>(prog.back()));
    REQUIRE(std::get<Acc>(prog.back()).value == 6);

    Cpu cpu(prog);

    cpu.execute();

    REQUIRE(cpu.acc == 5);
  }

  SECTION("part 2") {
    Cpu cpu(prog);
    REQUIRE(cpu.next_command_to_mutate(0) == 0);
    REQUIRE(cpu.next_command_to_mutate(1) == 2);

    REQUIRE(cpu.next_command_to_mutate(3) == 4);
    REQUIRE(cpu.next_command_to_mutate(5) == 7);
    REQUIRE(cpu.next_command_to_mutate(8) == 9);

    int ptr = 0;
    Cpu m1 = cpu.mutate(ptr);
    m1.execute();
    REQUIRE(m1.finished() == false);
    REQUIRE(ptr == 0);

    ptr = 5;
    Cpu m2 = cpu.mutate(ptr);
    m2.execute();
    REQUIRE(m2.finished() == true);
    REQUIRE(m2.acc == 8);

    Program fixed_program = fix_program(prog);
    Cpu fixed_cpu(fixed_program);
    fixed_cpu.execute();
    REQUIRE(fixed_cpu.acc == 8);
  }
};

TEST_CASE("day 8  ") {
  std::ifstream in(DATA_DIR "/dataset/input_08.txt", std::ifstream::in);
  REQUIRE(in.good());

  auto prog = parse_commands(in);
  Cpu cpu(prog);
  cpu.execute();

  std::cout << " day 8 part 1 : " << cpu.acc << "\n";

  Program fixed_program = fix_program(prog);
  Cpu fixed_cpu(fixed_program);
  fixed_cpu.execute();

  std::cout << " day 8 part 2 : " << fixed_cpu.acc << "\n";
}