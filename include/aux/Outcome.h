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
 * @file Outcome.h
 * @brief Header for class Outcome
 * @version 0.1
 *
 */
#pragma once
#include <cassert> // assert
#include <variant> // std::variant

#include "aux/Error.h"

namespace pink {

/**
 * @brief A container holding one of two alternatives, but not both.
 *
 * @tparam T the first alternative's type
 * @tparam U the second alternative's type
 */
template <class T, class U = Error> class Outcome {
private:
  std::variant<std::monostate, T, U> member;

public:
  Outcome() noexcept  = default;
  ~Outcome() noexcept = default;
  Outcome(const T &one) noexcept
      : member(one) {}
  Outcome(T &&one) noexcept
      : member(std::forward<T>(one)) {}
  Outcome(const U &two) noexcept
      : member(two) {}
  Outcome(U &&two) noexcept
      : member(std::forward<U>(two)) {}
  Outcome(const Outcome &other) noexcept
      : member(other.member) {}
  Outcome(Outcome &&other) noexcept
      : member(std::move(other.member)) {}

  auto operator=(const T &element) noexcept -> Outcome & {
    member = element;
    return *this;
  }

  auto operator=(T &&element) noexcept -> Outcome & {
    member = std::move(element);
    return *this;
  }

  auto operator=(const U &element) noexcept -> Outcome & {
    member = element;
    return *this;
  }

  auto operator=(U &&element) noexcept -> Outcome & {
    member = std::move(element);
    return *this;
  }

  auto operator=(const Outcome &other) noexcept -> Outcome & {
    if (this == &other) {
      return *this;
    }
    member = other.member;
    return *this;
  }

  auto operator=(Outcome &&other) noexcept -> Outcome & {
    member = std::move(other.member);
    return *this;
  }

  operator bool() { return std::holds_alternative<T>(member); }

  auto GetWhich() -> bool { return std::holds_alternative<T>(member); }

  auto GetFirst() -> T & {
    assert(std::holds_alternative<T>(member));
    return std::get<T>(member);
  }

  auto GetSecond() -> U & {
    assert(std::holds_alternative<U>(member));
    return std::get<U>(member);
  }
};

} // namespace pink
