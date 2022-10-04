/**
 * @file PointerType.h
 * @brief Header for class PointerType
 * @version 0.1
 */
#pragma once
#include <string>

#include "type/Type.h"
/*
 *
 * So there are a few points of confusion for pointers.
 *
 * 1) what is a pointer in LLVM?
 *  -) that is actually easy to answer, a pointer in LLVM is like a
 *      pointer in C. and, in fact we are already using them within the
 *      language within the Variable, and VarRef structures. That is
 *      because CreateAlloca, and CreateGlobal return pointers to the
 *      memory they allocate on the stack or in global memory respectively.
 *      this means that when Variable is bound to a Value*, that Value* is
 *      either a Pointer to memory, or a Constant value directly.
 *      this means that we can simply construct Pointers using the same
 *      information that we use to construct the Value* that a Variable is
 *      bound to.
 *    this however raises our second question:
 *  2) How can we store and load pointer values into memory in LLVM?
 *     that is, if we are already using Pointers to properly Load/Store in the
 *     language, how can we modify the Load/Store process to store the
 *     pointer itself, instead of the data it is pointing to?
 *     -) the answer to that is with the PtrToInt, and IntToPtr instructions
 *          which are provided by LLVM, with the PtrToInt instruction we can
 * convert what is normaly a pointer value into an integer value, and then we
 * can load/store that integer like we would any other integer. and conversely,
 * given an integer value associated with a pointer type we can use IntToPtr to
 * retreive the Pointer object such that we can Load/Store the data which the
 * pointer is referencing.
 *
 *          in this way PtrToInt acts in a manner that makes it good for
 *          implementing the address of value operator (unary &),
 *          and IntToPtr acts in a manner that makes it good for implementing
 *          the value of address operator (unary * ).
 *
 *  This is what leads us to our solution, namely, Pointers do not exist in
 *  the structure of the terms in the language, that is as an Ast node, but
 *  as values which the Ast nodes are constructing/destructing.
 *
 *  int main() {
 *    x := 0;
 *    y := &x;
 *    *y = 5;
 *    x;
 *  }
 */

namespace pink {
/**
 * @brief Represents the Type of a Pointer
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

  /**
   * @brief Implements LLVM style [RTTI] for this class
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
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Type *, Error> override;
};
} // namespace pink
