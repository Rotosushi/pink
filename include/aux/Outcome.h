/**
 * @file Outcome.h
 * @brief Header for class Outcome
 * @version 0.1
 *
 */
#pragma once
#include "aux/Error.h"
#include <utility> //

namespace pink {

/**
 * @brief A container holding one of two alternatives, but not both.
 *
 * \warning this class cannot be typed (legally constructed) if both alternative
 * types are the same type. This is fine however, because if you want to
 * return the same type in two different states to represent the return
 * value of a given function, that can be accomplished by simply using that
 * type directly as the return value of that function.
 *
 * @tparam T the first alternative's type
 * @tparam U the second alternative's type
 */
template <class T, class U> class Outcome {
private:
  /**
   * @brief which alternative is currently active
   * true when the first alternative is active
   * false when the second alternative is active
   */
  bool which;

  /**
   * @brief the first alternative
   *
   */
  T one;

  /**
   * @brief the second alternative
   *
   */
  U two;

public:
  /**
   * @brief there is no way to construct an Outcome without providing one of the
   * alternatives
   *
   */
  Outcome() = delete;

  /**
   * @brief Destroy the Outcome
   *
   */
  ~Outcome() = default;

  /**
   * @brief Construct a new Outcome, holding a first alternative equal to t
   *
   * @param one the value of the first alternative
   */
  Outcome(const T &one) : which(true), one(one), two() {}

  /**
   * @brief Construct a new Outcome, holding a first alternative equal to t
   *
   * @param one the value of the first alternative
   */
  Outcome(T &&one) : which(true), one(std::forward<T>(one)), two() {}

  /**
   * @brief Construct a new Outcome, holding a second alternative equal to u
   *
   * @param two the value of the second alternative
   */
  Outcome(const U &two) : which(false), one(), two(two) {}

  /**
   * @brief Construct a new Outcome, holding a second alternative equal to u
   *
   * @param two the value of the second alternative
   */
  Outcome(U &&two) : which(false), one(), two(std::forward<U>(two)) {}

  /**
   * @brief Construct a new Outcome, equal to another outcome
   *
   * @param other the other Outcome to copy the contents of
   */
  Outcome(const Outcome &other) : which(other.which) {
    if (which) {
      one = other.one;
    } else {
      two = other.two;
    }
  }

  /**
   * @copydoc Outcome::Outcome(const Outcome& other)
   */
  Outcome(Outcome &&other) noexcept : which(other.which) {
    if (which) {
      one = other.one;
    } else {
      two = other.two;
    }
  }

  /**
   * @brief Assign this Outcome to the value of the given first alternative t
   *
   * @param one the value of the first alternative
   * @return Outcome& this Outcome
   */
  auto operator=(const T &elem) -> Outcome & {
    which = true;
    one = elem;
    return *this;
  }

  /**
   * @brief Assign this Outcome to the value of the given second alternative u
   *
   * @param two the value of the second alternative
   * @return Outcome& this Outcome
   */
  auto operator=(const U &elem) -> Outcome & {
    which = false;
    two = elem;
    return *this;
  }

  /**
   * @brief Assign this Outcome to the value of another Outcome
   *
   * @param other the other Outcome to copy
   * @return Outcome& this Outcome
   */
  auto operator=(const Outcome &other) -> Outcome & {
    which = other.which;

    if (which) {
      one = other.one;
    } else {
      two = other.two;
    }

    return *this;
  }

  /**
   * @copydoc Outcome::operator=(const Outcome& other)
   */
  auto operator=(Outcome &&other) noexcept -> Outcome & {
    which = other.which;

    if (which) {
      one = std::move(other.one);
    } else {
      two = std::move(other.two);
    }

    return *this;
  }

  /**
   * @brief returns which alternative is held
   *
   * @return true when the first alternative is held
   * @return false when the second alternative is held
   */
  operator bool() { return which; }

  /**
   * @brief Get which alternative is held
   *
   * @return true when the first alternative is held
   * @return false when the second alternative is held
   */
  auto GetWhich() -> bool { return which; }

  /**
   * @brief Get the first alternative
   *
   * @return T& the member t
   */
  auto GetFirst() -> T & {
    if (which) {
      return one;
    }

    FatalError("Bad Outcome Access", __FILE__, __LINE__);
    return one; // suppress warning
  }

  /**
   * @brief Get the second alternative
   *
   * @return U& the member u
   */
  auto GetSecond() -> U & {
    if (which) {
      FatalError("Bad Outcome Access", __FILE__, __LINE__);
      return two; // suppress warning
    }
    return two;
  }
};
} // namespace pink
