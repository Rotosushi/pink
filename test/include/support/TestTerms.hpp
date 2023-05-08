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
#include <random>
#include <sstream>

#include "aux/Environment.h"
#include "type/All.h"

/*
  This is a reimplementation of the set of valid
  terms in pink. This design allows for greater
  code reuse accross the test rigs.
*/

template <typename T> struct Expected {
  std::stringstream   term;
  pink::Type::Pointer type;
  T                   value;

  Expected(std::stringstream term, pink::Type::Pointer type, T value)
      : term(std::move(term)),
        type(type),
        value(value) {}
};

template <typename T>
using TermGenFn = Expected<T> (*)(pink::CompilationUnit &unit);

/*
  Used as a helper function to transfrom a valid language term into
  a function so it can be used within the Codegen tests.
*/
template <typename T>
inline auto MakeFunction(pink::CompilationUnit &unit,
                         TermGenFn<T>           term,
                         std::string_view       last_term = " result;\n",
                         std::string_view fn_name = "main") -> Expected<T> {
  auto expected = term(unit);

  std::stringstream main;
  main << "fn " << fn_name << "() {" << expected.term.str() << last_term << "}";

  auto type = unit.GetFunctionType({}, expected.type, {});
  return {std::move(main), type, expected.value};
}

inline auto Term0001(pink::CompilationUnit &unit) -> Expected<int> {
  std::random_device            seed;
  std::uniform_int_distribution dist{0, 100};
  auto                          value = dist(seed);

  std::stringstream term;
  term << "result := ";
  term << value;
  term << ";";

  return Expected{std::move(term), unit.GetIntType({}), value};
}

inline auto Term0002(pink::CompilationUnit &unit) -> Expected<bool> {
  std::random_device          seed;
  std::bernoulli_distribution dist;
  auto                        value = dist(seed);

  std::stringstream term;
  term << "result := ";
  term << (value ? "true" : "false");
  term << ";";

  return {std::move(term), unit.GetBoolType({}), value};
}
