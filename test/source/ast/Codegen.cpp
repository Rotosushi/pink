// Copyright (C) 2023 Cade Weinberg
//
// This file is part of pink.
//
// pink is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// pink is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pink.  If not, see <http://www.gnu.org/licenses/>.
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>     // errno
#include <cstdio>     // fopen, fputs
#include <cstring>    // std::strerror
#include <filesystem> // std::filesystem::path
#include <fstream>    // std::fstream
#include <random>     // std::random_device
#include <sstream>    // std::stringstream
#include <vector>     // std::vector

#include "catch2/catch_test_macros.hpp"

namespace fs = std::filesystem;

#include "support/FatalError.h"

static auto CreateUniqueTempFilename() -> fs::path {
  auto temp_path = []() {
    std::error_code errc;
    fs::path        temp_path = fs::temp_directory_path(errc);
    if (errc) {
      pink::FatalError(errc);
    }
    return temp_path;
  };

  auto temp_filename = []() {
    std::random_device seed;
    std::stringstream  sstream;
    sstream << "temp" << seed();
    return sstream.str();
  };

  auto temp_filepath = [&]() { return temp_path() / temp_filename(); };

  auto unique_tempfile = [&]() {
    fs::path temp_file = temp_filepath();
    while (fs::exists(temp_file)) {
      temp_file = temp_filepath();
    }
    return temp_file;
  };

  return unique_tempfile();
}

static void EmitTempFile(const std::string &contents,
                         const fs::path    &tempfile) {
  auto file = fopen(tempfile.c_str(), "w"); // NOLINT

  if (file == nullptr) {
    pink::FatalError(std::strerror(errno));
  }

  if (fputs(contents.c_str(), file) == EOF) {
    pink::FatalError(std::strerror(errno));
  }

  if (fclose(file) == EOF) {
    pink::FatalError(std::strerror(errno));
  }
}

static auto Execute(char const *program, char *const *argv) -> int {
  int result = -1;

  pid_t pid = fork();

  if (pid < 0) {
    pink::FatalError(std::strerror(errno));
  }

  if (pid == 0) {
    if (execv(program, argv) < 0) {
      pink::FatalError(std::strerror(errno));
    }
  } else {
    // wait for child process to execute
    siginfo_t siginfo;
    // normally the signed to unsigned conversion is
    // bad here, (pid_t -> id_t) however all negative
    // values of pid_t are handled above, so any pid_t
    // that reaches this point must be a positive integer.
    // which maps correctly into an unsigned integer.
    int       wait = waitid(P_PID, static_cast<id_t>(pid), &siginfo, WEXITED);

    if (wait < 0) {
      pink::FatalError(std::strerror(errno));
    }

    if (siginfo.si_code == CLD_EXITED) {
      result = siginfo.si_value.sival_int;
    }
  }

  return result;
}

static auto CompileAndRunProgram(const std::string &contents)
    -> std::optional<int> {
  auto temp_program = CreateUniqueTempFilename();

  auto temp_file = temp_program;
  temp_file.replace_extension(".p");

  EmitTempFile(contents, temp_file);

  std::vector<char const *> compile;
  compile.reserve(3);
  compile.emplace_back("./pink");
  compile.emplace_back(temp_file.c_str());
  compile.emplace_back(nullptr);

  // need to cast char const ** to char *const * in order to pass args to execv
  // execv respects char const * const *
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
  auto result = Execute(compile[0], const_cast<char *const *>(compile.data()));

  if (result == EXIT_FAILURE) {
    return {};
  }

  std::vector<char const *> execute;
  execute.reserve(2);
  execute.emplace_back(temp_program.c_str());
  execute.emplace_back(nullptr);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
  result = Execute(execute[0], const_cast<char *const *>(execute.data()));

  std::error_code errc;
  fs::remove(temp_program, errc);
  if (errc) {
    pink::FatalError(errc);
  }

  fs::remove(temp_file, errc);
  if (errc) {
    pink::FatalError(errc);
  }

  return result;
}

// NOLINTBEGIN
TEST_CASE("ast/Codegen: Integer", "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 100};
  auto                          value = dist(gen);

  std::string main  = "fn main() { ";
  main             += std::to_string(value);
  main             += "; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(result.value() == value);
}

TEST_CASE("ast/Codegen: Boolean", "[integration][ast][ast/action]") {
  std::random_device          seed;
  std::mt19937                gen{seed()};
  std::bernoulli_distribution dist;
  auto                        value = dist(gen);

  std::string main  = "fn main() { ";
  main             += value ? "true" : "false";
  main             += "; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == value);
}

TEST_CASE("ast/Codegen: Nil", "[integration][ast][ast/action]") {
  std::string main = "fn main() { nil; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(result.value() == 0);
}

TEST_CASE("ast/Codegen: Variable", "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 100};
  auto                          value = dist(gen);

  std::string main  = "fn main() { a :=";
  main             += std::to_string(value);
  main             += "; a; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(result.value() == value);
}

/*
  I would like to add a test case for unop negate,
  however the way we read the return value of a
  child process destroys the negative value.
  i don't know a way around that.
  so this test will have to be performed within a combined
  test, "a := b + -c" or something similar.
*/

TEST_CASE("ast/Codegen: Unop not", "[integration][ast][ast/action]") {
  std::random_device          seed;
  std::mt19937                gen{seed()};
  std::bernoulli_distribution dist;
  auto                        value = dist(gen);

  std::string main  = "fn main() { !";
  main             += value ? "true" : "false";
  main             += "; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == !value);
}

TEST_CASE("ast/Codegen: Addition", "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 50};
  auto                          a = dist(gen);
  auto                          b = dist(gen);

  std::string main  = "fn main() { a :=";
  main             += std::to_string(a);
  main             += ";\n b := ";
  main             += std::to_string(b);
  main             += ";\n a + b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(result.value() == (a + b));
}

TEST_CASE("ast/Codegen: Subtraction", "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 100};
  auto                          a = dist(gen);
  auto                          b = dist(gen);
  if (b > a) {
    std::swap(a, b);
  }

  std::string main  = "fn main() { a :=";
  main             += std::to_string(a);
  main             += ";\n b := ";
  main             += std::to_string(b);
  main             += ";\n a - b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(result.value() == (a - b));
}

TEST_CASE("ast/Codegen: Multiplication", "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 10};
  auto                          a = dist(gen);
  auto                          b = dist(gen);

  std::string main  = "fn main() { a :=";
  main             += std::to_string(a);
  main             += ";\n b := ";
  main             += std::to_string(b);
  main             += ";\n a * b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(result.value() == (a * b));
}

TEST_CASE("ast/Codegen: Division", "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{1, 100};
  auto                          a = dist(gen);
  auto                          b = dist(gen);

  std::string main  = "fn main() { a :=";
  main             += std::to_string(a);
  main             += ";\n b := ";
  main             += std::to_string(b);
  main             += ";\n a / b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(result.value() == (a / b));
}

TEST_CASE("ast/Codegen: Modulus", "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{1, 100};
  auto                          a = dist(gen);
  auto                          b = dist(gen);

  std::string main  = "fn main() { a :=";
  main             += std::to_string(a);
  main             += ";\n b := ";
  main             += std::to_string(b);
  main             += ";\n a % b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(result.value() == (a % b));
}

TEST_CASE("ast/Codegen: And", "[integration][ast][ast/action]") {
  std::random_device          seed;
  std::mt19937                gen{seed()};
  std::bernoulli_distribution dist;
  auto                        a = dist(gen);
  auto                        b = dist(gen);

  std::string main  = "fn main() { a := ";
  main             += a ? "true" : "false";
  main             += ";\n b := ";
  main             += b ? "true" : "false";
  main             += ";\n a & b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == (a && b));
}

TEST_CASE("ast/Codegen: Or", "[integration][ast][ast/action]") {
  std::random_device          seed;
  std::mt19937                gen{seed()};
  std::bernoulli_distribution dist;
  auto                        a = dist(gen);
  auto                        b = dist(gen);

  std::string main  = "fn main() { a := ";
  main             += a ? "true" : "false";
  main             += ";\n b := ";
  main             += b ? "true" : "false";
  main             += ";\n a | b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == (a || b));
}

TEST_CASE("ast/Codegen: Equality<Integer, Integer>",
          "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 100};
  auto                          a = dist(gen);
  auto                          b = dist(gen);

  std::string main  = "fn main() { a :=";
  main             += std::to_string(a);
  main             += ";\n b := ";
  main             += std::to_string(b);
  main             += ";\n a == b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == (a == b));
}

TEST_CASE("ast/Codegen: Equality<Boolean, Boolean>",
          "[integration][ast][ast/action]") {
  std::random_device          seed;
  std::mt19937                gen{seed()};
  std::bernoulli_distribution dist;
  auto                        a = dist(gen);
  auto                        b = dist(gen);

  std::string main  = "fn main() { a := ";
  main             += a ? "true" : "false";
  main             += ";\n b := ";
  main             += b ? "true" : "false";
  main             += ";\n a == b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == (a == b));
}

TEST_CASE("ast/Codegen: Inequality<Integer, Integer>",
          "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 100};
  auto                          a = dist(gen);
  auto                          b = dist(gen);

  std::string main  = "fn main() { a :=";
  main             += std::to_string(a);
  main             += ";\n b := ";
  main             += std::to_string(b);
  main             += ";\n a != b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == (a != b));
}

TEST_CASE("ast/Codegen: Inequality<Boolean, Boolean>",
          "[integration][ast][ast/action]") {
  std::random_device          seed;
  std::mt19937                gen{seed()};
  std::bernoulli_distribution dist;
  auto                        a = dist(gen);
  auto                        b = dist(gen);

  std::string main  = "fn main() { a := ";
  main             += a ? "true" : "false";
  main             += ";\n b := ";
  main             += b ? "true" : "false";
  main             += ";\n a != b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == (a != b));
}

TEST_CASE("ast/Codegen: Less than", "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 100};
  auto                          a = dist(gen);
  auto                          b = dist(gen);

  std::string main  = "fn main() { a :=";
  main             += std::to_string(a);
  main             += ";\n b := ";
  main             += std::to_string(b);
  main             += ";\n a < b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == (a < b));
}

TEST_CASE("ast/Codegen: Less than or Equal", "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 100};
  auto                          a = dist(gen);
  auto                          b = dist(gen);

  std::string main  = "fn main() { a :=";
  main             += std::to_string(a);
  main             += ";\n b := ";
  main             += std::to_string(b);
  main             += ";\n a <= b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == (a <= b));
}

TEST_CASE("ast/Codegen: Greater than", "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 100};
  auto                          a = dist(gen);
  auto                          b = dist(gen);

  std::string main  = "fn main() { a :=";
  main             += std::to_string(a);
  main             += ";\n b := ";
  main             += std::to_string(b);
  main             += ";\n a > b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == (a > b));
}

TEST_CASE("ast/Codegen: Greater than or equal",
          "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 100};
  auto                          a = dist(gen);
  auto                          b = dist(gen);

  std::string main  = "fn main() { a :=";
  main             += std::to_string(a);
  main             += ";\n b := ";
  main             += std::to_string(b);
  main             += ";\n a >= b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == (a >= b));
}

TEST_CASE("ast/Codegen: Addition and Unop negate",
          "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 50};
  auto                          a = dist(gen);
  auto                          b = dist(gen);
  if (a < b) {
    std::swap(a, b);
  }

  std::string main  = "fn main() { a :=";
  main             += std::to_string(a);
  main             += ";\n b := ";
  main             += std::to_string(b);
  main             += ";\n a + -b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(result.value() == (a + -b));
}

TEST_CASE("ast/Codegen: And and Unop Not", "[integration][ast][ast/action]") {
  std::random_device          seed;
  std::mt19937                gen{seed()};
  std::bernoulli_distribution dist;
  auto                        a = dist(gen);
  auto                        b = dist(gen);

  std::string main  = "fn main() { a := ";
  main             += a ? "true" : "false";
  main             += ";\n b := ";
  main             += b ? "true" : "false";
  main             += ";\n a & !b; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == (a && !b));
}

TEST_CASE("ast/Codegen: Equality<Integer, Integer> and Addition Precedence",
          "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 100};
  auto                          a = dist(gen);
  auto                          b = dist(gen);
  auto                          c = dist(gen);
  auto                          d = dist(gen);

  std::string main  = "fn main() { a :=";
  main             += std::to_string(a);
  main             += ";\n b := ";
  main             += std::to_string(b);
  main             += ";\n c := ";
  main             += std::to_string(c);
  main             += ";\n d := ";
  main             += std::to_string(d);
  main             += ";\n a + b == c + d; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == ((a + b) == (c + d)));
}

TEST_CASE("ast/Codegen: Equality<Integer, Integer> and Operator Precedence",
          "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 10};
  auto                          a = dist(gen);
  auto                          b = dist(gen);
  auto                          c = dist(gen);
  auto                          d = dist(gen);
  auto                          e = dist(gen);
  auto                          f = dist(gen);

  std::string main  = "fn main() { a :=";
  main             += std::to_string(a);
  main             += ";\n b := ";
  main             += std::to_string(b);
  main             += ";\n c := ";
  main             += std::to_string(c);
  main             += ";\n d := ";
  main             += std::to_string(d);
  main             += ";\n e := ";
  main             += std::to_string(e);
  main             += ";\n f := ";
  main             += std::to_string(f);
  main             += ";\n a + b * c == d + e * f; }";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(static_cast<bool>(result.value()) == ((a + b * c) == (d + e * f)));
}

TEST_CASE("ast/Codegen: Assignment", "[integration][ast][ast/action]") {
  std::random_device            seed;
  std::mt19937                  gen{seed()};
  std::uniform_int_distribution dist{0, 100};
  auto                          a = dist(gen);

  std::string main  = "fn main() { a := 0;\n a = ";
  main             += std::to_string(a);
  main             += ";\n a;\n}";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(result.value() == a);
}

TEST_CASE("ast/Codegen: Tuple Dot Access", "[integration][ast][ast/action]") {
  std::random_device                         seed;
  std::mt19937                               gen{seed()};
  std::uniform_int_distribution              dist{0, 100};
  std::uniform_int_distribution<std::size_t> access_range{0, 5};
  std::array                                 elements =
      {dist(gen), dist(gen), dist(gen), dist(gen), dist(gen), dist(gen)};
  auto element = access_range(gen);

  std::string main = "fn main() { a := (";
  for (std::size_t index = 0; index < elements.size(); ++index) {
    main += std::to_string(elements[index]);

    if (index < (elements.size() - 1)) {
      main += ", ";
    }
  }
  main += ");\n a.";
  main += std::to_string(element);
  main += ";\n}";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(result.value() == elements[element]);
}

TEST_CASE("ast/Codegen: Array Subscript Access",
          "[integration][ast][ast/action]") {
  std::random_device                         seed;
  std::mt19937                               gen{seed()};
  std::uniform_int_distribution              dist{0, 100};
  std::uniform_int_distribution<std::size_t> access_range{0, 5};
  std::array                                 elements =
      {dist(gen), dist(gen), dist(gen), dist(gen), dist(gen), dist(gen)};
  auto element = access_range(gen);

  std::string main = "fn main() { a := [";
  for (std::size_t index = 0; index < elements.size(); ++index) {
    main += std::to_string(elements[index]);

    if (index < (elements.size() - 1)) {
      main += ", ";
    }
  }
  main += "];\n a[";
  main += std::to_string(element);
  main += "];\n}";

  auto result = CompileAndRunProgram(main);

  REQUIRE(result.has_value());
  CHECK(result.value() == elements[element]);
}
// NOLINTEND