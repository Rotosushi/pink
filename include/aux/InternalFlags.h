/**
 * @file TypecheckFlags.h
 * @brief Header for class TypecheckFlags
 * @version 0.1
 *
 */
#pragma once
#include <bitset>

#include "support/BitsPerByte.h"

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
