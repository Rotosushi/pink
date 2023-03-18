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

namespace pink {

/**
 * @brief A container holding one of two alternatives, but not both.
 *
 * @tparam T the first alternative's type
 * @tparam U the second alternative's type
 */
template <class T, class U> class Outcome {
private:
  std::variant<std::monostate, T, U> member;

public:
  /**
   * @brief constructs an empty variant
   *
   * \warning a default constructed Outcome is not safe to access
   */
  Outcome() noexcept = default;

  /**
   * @brief Destroy the Outcome
   *
   */
  ~Outcome() noexcept = default;

  /**
   * @brief Construct a new Outcome, holding a first alternative equal to t
   *
   * @param one the value of the first alternative
   */
  Outcome(const T &one) noexcept
      : member(one) {}

  /**
   * @brief Construct a new Outcome, holding a first alternative equal to t
   *
   * @param one the value of the first alternative
   */
  Outcome(T &&one) noexcept
      : member(std::forward<T>(one)) {}

  /**
   * @brief Construct a new Outcome, holding a second alternative equal to u
   *
   * @param two the value of the second alternative
   */
  Outcome(const U &two) noexcept
      : member(two) {}

  /**
   * @brief Construct a new Outcome, holding a second alternative equal to u
   *
   * @param two the value of the second alternative
   */
  Outcome(U &&two) noexcept
      : member(std::forward<U>(two)) {}

  /**
   * @brief Construct a new Outcome, equal to another outcome
   *
   * @param other the other Outcome to copy the contents of
   */
  Outcome(const Outcome &other) noexcept
      : member(other.member) {}

  /**
   * @copydoc Outcome::Outcome(const Outcome& other)
   */
  Outcome(Outcome &&other) noexcept
      : member(std::move(other.member)) {}

  /**
   * @brief Assign this Outcome to the value of the given first alternative t
   *
   * @param one the value of the first alternative
   * @return Outcome& this Outcome
   */
  auto operator=(const T &element) noexcept -> Outcome & {
    member = element;
    return *this;
  }

  auto operator=(T &&element) noexcept -> Outcome & {
    member = std::move(element);
    return *this;
  }

  /**
   * @brief Assign this Outcome to the value of the given second alternative u
   *
   * @param two the value of the second alternative
   * @return Outcome& this Outcome
   */
  auto operator=(const U &element) noexcept -> Outcome & {
    member = element;
    return *this;
  }

  auto operator=(U &&element) noexcept -> Outcome & {
    member = std::move(element);
    return *this;
  }

  /**
   * @brief Assign this Outcome to the value of another Outcome
   *
   * @param other the other Outcome to copy
   * @return Outcome& this Outcome
   */
  auto operator=(const Outcome &other) noexcept -> Outcome & {
    member = other.member;
    return *this;
  }

  /**
   * @copydoc Outcome::operator=(const Outcome& other)
   */
  auto operator=(Outcome &&other) noexcept -> Outcome & {
    member = std::move(other.member);
    return *this;
  }

  /**
   * @brief returns which alternative is held
   *
   * @return true when the first alternative is held
   * @return false when the second alternative is held
   */
  operator bool() { return std::holds_alternative<T>(member); }

  /**
   * @brief Get which alternative is held
   *
   * @return true when the first alternative is held
   * @return false when the second alternative is held
   */
  auto GetWhich() -> bool { return std::holds_alternative<T>(member); }

  /**
   * @brief Get the first alternative
   *
   * @return T& the member t
   */
  auto GetFirst() -> T & {
    assert(std::holds_alternative<T>(member));
    return std::get<T>(member);
  }

  /**
   * @brief Get the second alternative
   *
   * @return U& the member u
   */
  auto GetSecond() -> U & {
    assert(std::holds_alternative<U>(member));
    return std::get<U>(member);
  }
};

} // namespace pink
