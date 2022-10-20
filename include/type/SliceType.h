#pragma once

#include "type/Type.h"

namespace pink {
/**
 * @brief The type of slices
 *
 *
 * a slice is a pointer composed together with the
 * size of the allocation it points too.
 *
 * An Array can be coerced into a slice safely.
 * the only consideration is the fact that we
 * store an array as the length plus the array,
 * so a slice pointing to the array will be a
 * copy of the length plus a pointer to the array
 * allocation. Thus using a slice in this way incurs
 * a bit of overhead compared to direct array access
 * within a local scope.
 * however passing an array to a function using a slice
 * is very efficient compared to passing the array
 * itself by value.
 *
 * an array in the return slot of a function, as the language
 * is currently defined must have compile time known size.
 * and then it can be allocated in the calling scope and
 * simply written to in the callee scope. And this should
 * work up a chain of calls.
 *
 */
class SliceType : public Type {
public:
  Type *pointee_type;

  SliceType(Type *pointee_type);

  ~SliceType() override = default;

  SliceType(const SliceType &other) = default;

  SliceType(SliceType &&other) = default;

  auto operator=(const SliceType &other) -> SliceType & = default;

  auto operator=(SliceType &&other) -> SliceType & = default;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param type the type being tested
   * @return true if type *is* an instance of SliceType
   * @return false if type *is not* an instance of SliceType
   */
  static auto classof(const Type *type) -> bool;

  /**
   * @brief Computes if other is equivalent to this SliceType
   *
   * @param other the other type
   * @return true if other *is* equivalent to this SliceType
   * @return false if other  *is not* equivalent to this SliceType
   */
  auto EqualTo(Type *other) const -> bool override;

  /**
   * @brief Compute the cannonical string representation of this SliceType
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the llvm::Type equivalent to this SliceType
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Type *, Error> if true the llvm::Type, if false the
   * Error encountered
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Type *, Error> override;
};
} // namespace pink