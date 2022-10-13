/**
 * @file Flags.h
 * @brief Header for class Flags
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
class Flags {
private:
  /**
   * @brief records the kind and position of each available flag within the
   * [flags](#Flags::flags) member.
   *
   * we use the (enum -> unsigned int) implicit conversion to encode the
   * position of each flag. that is, the first enum member defined gets position
   * 0, the next position 1, and so on.
   */
  enum Kind : uint8_t {
    LHSOfAssignment,
    inAddressOf,
    inDereferencePtr,
  };

  /**
   * @brief records the state of each available flag.
   *
   * the choice of size of bitset is reletive to the number of
   * flags we need to record. since this is always knowable at
   * compile time, we never need to consider using std::vector<bool>
   * or boost::dynamic_bitset<>.
   */
  std::bitset<sizeof(Kind) * bits_per_byte> flags;

public:
  /**
   * @brief Construct a new Flags object
   *
   */
  Flags() = default;

  /**
   * @brief Destroy the Flags object
   *
   */
  ~Flags() = default;

  Flags(Flags &other) = default;

  Flags(Flags &&other) = default;

  auto operator=(Flags const &other) -> Flags & = default;

  auto operator=(Flags &&other) -> Flags & = default;

  /**
   * @brief the state of the LHSOfAssignment flag
   *
   * @return true when we *are* on the left hand side of an Assignment term
   * @return false when we *are not* on the left hand side of an Assignment term
   */
  [[nodiscard]] auto OnTheLHSOfAssignment() const -> bool;

  /**
   * @brief set the state of the LHSOfAssignment flag to "state"
   *
   * @param state the new state of the LHSOfAssignment flag
   * @return bool the new state of the LHSOfAssignment flag
   */
  auto OnTheLHSOfAssignment(bool state) -> bool;

  /**
   * @brief the state of the inAddressOf flag
   *
   * @return true when we *are* within an AddressOf "&" Unop
   * @return false when we *are not* within an AddressOf "&" Unop
   */
  [[nodiscard]] auto WithinAddressOf() const -> bool;

  /**
   * @brief set the state of the inAddressOf flag to "state"
   *
   * @param state the new state of the inAddressOf flag
   * @return bool the new state of the inAddressOf flag
   */
  auto WithinAddressOf(bool state) -> bool;

  /**
   * @brief the state of the inDereferencePtr flag
   *
   * @return true when we *are* within a DereferencePtr "*" Unop
   * @return false when we *are not* within a DereferencePtr "*" Unop
   */
  [[nodiscard]] auto WithinDereferencePtr() const -> bool;

  /**
   * @brief set the state of the inDereferencePtr flag to "state"
   *
   * @param state the new state of the inDereferencePtr flag
   * @return bool the new state of the inDereferencePtr flag
   */
  auto WithinDereferencePtr(bool state) -> bool;
};
} // namespace pink
