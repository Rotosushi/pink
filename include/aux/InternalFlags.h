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

/**
 * @file TypecheckFlags.h
 * @brief Header for class TypecheckFlags
 * @version 0.1
 *
 */
#pragma once
#include <bitset>

namespace pink {

/**
 * @brief Holds the state of flags relevant to the inner workings of the
 * compiler, which are *not* dials to be tuned by the user.
 *
 * These flags are only defined when there is no other sensible way of knowing
 * the particular bit of information the flag encodes by any other means
 * available to a typechecking or codegeneration routine.
 */
class EnvironmentFlags {
private:
  enum Kind {
    LHSOfAssignment,
    inAddressOf,
    inDereferencePtr,
    inBindExpression,
    SIZE, // #NOTE! this must be the last member,
          // no enums may have an assigned value.
  };

  static constexpr auto bitset_size = Kind::SIZE;

  using Set = std::bitset<bitset_size>;

  Set set;

public:
  EnvironmentFlags()                                                  = default;
  ~EnvironmentFlags()                                                 = default;
  EnvironmentFlags(EnvironmentFlags &other)                           = default;
  EnvironmentFlags(EnvironmentFlags &&other)                          = default;
  auto operator=(EnvironmentFlags const &other) -> EnvironmentFlags & = default;
  auto operator=(EnvironmentFlags &&other) -> EnvironmentFlags      & = default;

  [[nodiscard]] auto OnTheLHSOfAssignment() const -> bool {
    return set[LHSOfAssignment];
  }
  auto OnTheLHSOfAssignment(bool state) -> bool {
    return set[LHSOfAssignment] = state;
  }

  [[nodiscard]] auto WithinAddressOf() const -> bool {
    return set[inAddressOf];
  }
  auto WithinAddressOf(bool state) -> bool { return set[inAddressOf] = state; }

  [[nodiscard]] auto WithinDereferencePtr() const -> bool {
    return set[inDereferencePtr];
  }
  auto WithinDereferencePtr(bool state) -> bool {
    return set[inDereferencePtr] = state;
  }

  [[nodiscard]] auto WithinBindExpression() const -> bool {
    return set[inBindExpression];
  }
  auto WithinBindExpression(bool state) -> bool {
    return set[inBindExpression] = state;
  }
};

} // namespace pink
