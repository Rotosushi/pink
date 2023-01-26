/**
 * @file TypecheckFlags.h
 * @brief Header for class TypecheckFlags
 * @version 0.1
 *
 */
#pragma once
#include <bitset>

#include "support/Support.h"

namespace pink {

/**
 * @brief Holds the state of flags relevant to the inner workings of the
 * compiler, which are *not* dials to be tuned by the user.
 *
 * These flags are only defined when there is no other sensible way of knowing
 * the particular bit of information the flags encodes by any other means
 * available to a typechecking or codegeneration routine.
 */
class TypecheckFlags {
private:
  /**
   * @brief records the kind and position of each available flag within the
   * [flags](#TypecheckFlags::flags) member.
   *
   * we use the (enum -> unsigned int) implicit conversion to encode the
   * position of each flag. that is, the first enum member defined gets position
   * 0, the next position 1, and so on.
   */
  enum Kind : char {
    LHSOfAssignment,
    inAddressOf,
    inDereferencePtr,
    inBindExpression,
  };

  static constexpr auto bitset_size = sizeof(Kind) * bits_per_byte;

  using Flags = std::bitset<bitset_size>;
  /**
   * @brief records the state of each available flag.
   *
   * the choice of size is reletive to the number of
   * flags we need to record. since this is always knowable at
   * compile time, we have a perfect use case for a std::bitset
   */
  Flags flags;

public:
  TypecheckFlags() = default;
  ~TypecheckFlags() = default;
  TypecheckFlags(TypecheckFlags &other) = default;
  TypecheckFlags(TypecheckFlags &&other) = default;
  auto operator=(TypecheckFlags const &other) -> TypecheckFlags & = default;
  auto operator=(TypecheckFlags &&other) -> TypecheckFlags & = default;

  [[nodiscard]] auto OnTheLHSOfAssignment() const -> bool;
  auto OnTheLHSOfAssignment(bool state) -> bool;

  [[nodiscard]] auto WithinAddressOf() const -> bool;
  auto WithinAddressOf(bool state) -> bool;

  [[nodiscard]] auto WithinDereferencePtr() const -> bool;
  auto WithinDereferencePtr(bool state) -> bool;

  [[nodiscard]] auto WithinBindExpression() const -> bool;
  auto WithinBindExpression(bool state) -> bool;
};

} // namespace pink
