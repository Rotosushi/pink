// Copyright (C) 2023 cadence
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

#include "catch2/catch_test_macros.hpp"

#include "aux/Error.h"

#include <random>

// a printed error consists of three lines
// the string associated with the error code +
// ":" +
// the description the error holds +
// newline +
// the bad source +
// newline +
// the highlight line
//
// essentially what we want to check is that
// between location.firstColumn and location.lastColumn
// the highlight line contains the character '^' and everywhere
// else in the line is a space character.
static auto ProperlyUnderlined(std::string    &description,
                               pink::Location &location) -> bool {
  if (description.empty()) {
    return false;
  }

  // walk past the first two newline characters
  bool   seen_newline = false;
  size_t index        = 0;
  while (true) {
    if (description[index] == '\n') {
      if (seen_newline) {
        break;
      }
      seen_newline = true;
    }
    index++;
  }

  while (true) {
    if (index < location.firstColumn) {
      if (description[index] != ' ') {
        return false;
      }
    } else if (index < location.lastColumn) {
      if (description[index] != '^') {
        return false;
      }
    } else { // index must be > location.lastColumn
      return true;
    }
  }
}

TEST_CASE("aux/Error", "[unit][aux]") {
  std::string  bad_source = "some erroneous input text";
  std::mt19937 generator;

  // generate some random location within the bad_source
  // to place the error highlight
  std::uniform_int_distribution<unsigned long> begin(0, bad_source.size());
  auto                                         error_begin = begin(generator);

  std::uniform_int_distribution<unsigned long> end(error_begin,
                                                   bad_source.size());
  auto                                         error_end = end(generator);

  pink::Location location = {1, error_begin, 1, error_end};

  SECTION("Error::Code::None") {
    auto        code = pink::Error::Code::None;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::EndOfFile") {
    auto        code = pink::Error::Code::EndOfFile;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::MissingSemicolon") {
    auto        code = pink::Error::Code::MissingSemicolon;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::MissingLParen") {
    auto        code = pink::Error::Code::MissingLParen;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::MissingRParen") {
    auto        code = pink::Error::Code::MissingRParen;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::MissingLBrace") {
    auto        code = pink::Error::Code::MissingLBrace;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::MissingRBrace") {
    auto        code = pink::Error::Code::MissingRBrace;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::MissingFn") {
    auto        code = pink::Error::Code::MissingFn;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::MissingFnName") {
    auto        code = pink::Error::Code::MissingFnName;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::MissingArgName") {
    auto        code = pink::Error::Code::MissingArgName;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::MissingArgColon") {
    auto        code = pink::Error::Code::MissingArgColon;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::MissingArgType") {
    auto        code = pink::Error::Code::MissingArgType;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::UnknownBinop") {
    auto        code = pink::Error::Code::UnknownBinop;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::UnknownUnop") {
    auto        code = pink::Error::Code::UnknownUnop;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::UnknownBasicToken") {
    auto        code = pink::Error::Code::UnknownBasicToken;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::TypeCannotBeCalled") {
    auto        code = pink::Error::Code::TypeCannotBeCalled;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::ArgNumMismatch") {
    auto        code = pink::Error::Code::ArgNumMismatch;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::ArgTypeMismatch") {
    auto        code = pink::Error::Code::ArgTypeMismatch;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::AssigneeTypeMismatch") {
    auto        code = pink::Error::Code::AssigneeTypeMismatch;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::NameNotBoundInScope") {
    auto        code = pink::Error::Code::NameNotBoundInScope;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::NameAlreadyBoundInScope") {
    auto        code = pink::Error::Code::NameAlreadyBoundInScope;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::ValueCannotBeAssigned") {
    auto        code = pink::Error::Code::ValueCannotBeAssigned;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
  SECTION("Error::Code::NonConstGlobalInit") {
    auto        code = pink::Error::Code::NonConstGlobalInit;
    pink::Error error(code, location);
    auto        description = error.ToString(bad_source);
    REQUIRE(error.code == code);
    REQUIRE(ProperlyUnderlined(description, location));
  }
}