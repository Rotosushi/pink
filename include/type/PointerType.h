/**
 * @file PointerType.h
 * @brief Header for class PointerType
 * @version 0.1
 */
#pragma once
#include <string>

#include "type/Type.h"
/*
 * 1/8/2023
 * So there are a few points of confusion for pointers.
 *
 * 1) what is a pointer in LLVM?
 *  -) conceptually: an integer value holding the address of another llvm value.
 *    this however raises our second question:
 *  2) How can we store and load pointer values into memory in LLVM?
 *   -) simply load and store them like regular values. that is
 *      when you construct a integer on the stack llvm gives you a
 *      pointer to that value back, so instead of loading the integer
 *      from the stack, simply use the pointer directly.
 */

namespace pink {
/**
 * @brief Represents the Type of a Pointer to a single value.
 *
 * pointers are integer values, so they can be assigned and
 * passed as parameters, but in the interest of memory safety
 * pointers only support indirection operation. that is unop [*]
 * a pointer can be constructed from any variable with
 * address of, that is unop [&].
 *
 * instead if you want pointer arithmetic you do that via slices,
 * which hold the addressable range they support. then the runtime
 * can check pointer math, and so can the compiler (if the values
 * are literals)
 *
 * a pointer to a slice may be coerced to a slice safely. and
 * any element within the range of a slice must have a valid address.
 *
 * however you may not convert a pointer into a slice.
 *
 * (#NOTE: some computer architechtures use memory mapped I/O
 * for their hardware devices, thus for device drivers it is
 * useful to construct a pointer to an arbitrary memory location.
 * this is something to be considered. if this valid use case
 * was not present I would be on board with saying that pointers
 * can only be constructed via the address of operator. as in there
 * would not be syntax to wite down a pointer literal with.)
 *
 *
 */
class PointerType : public Type {
public:
  /**
   * @brief The type of what the Pointer is pointing to, it's 'pointee'.
   *
   */
  Type *pointee_type;

  /**
   * @brief Construct a new PointerType
   *
   * @param pointee_type the Type of what this pointer is pointing to.
   */
  PointerType(Type *pointee_type);

  /**
   * @brief Destroy the PointerType
   *
   */
  ~PointerType() override = default;

  PointerType(const PointerType &other) = default;

  PointerType(PointerType &&other) = default;

  auto operator=(const PointerType &other) -> PointerType & = default;

  auto operator=(PointerType &&other) -> PointerType & = default;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param type the type being tested
   * @return true if type *is* an instance of PointerType
   * @return false if type *is not* an instance of PointerType
   */
  static auto classof(const Type *type) -> bool;

  /**
   * @brief Computes if other is equivalent to this PointerType
   *
   * @param other the other type
   * @return true if other *is* equivalent to this PointerType
   * @return false if other  *is not* equivalent to this PointerType
   */
  auto EqualTo(Type *other) const -> bool override;

  /**
   * @brief Compute the cannonical string representation of this PointerType
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the llvm::Type equivalent to this PointerType
   *
   * \note llvm::PointerType's do not store any Pointee type. if you want to
   * know the llvm version of this pointee type, you have to compute it directly
   * from this->pointee_type
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Type*, Error> if true, the llvm::Type equivalent to
   * this PointerType, if false then the Error encountered
   */
  [[nodiscard]] auto ToLLVM(const Environment &env) const
      -> llvm::Type * override;
};
} // namespace pink
