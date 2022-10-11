/**
 * @file Array.h
 * @brief Header for class Array
 * @version 0.1
 */
#pragma once
#include <vector>

#include "ast/Ast.h"

#include "llvm/IR/Value.h"

/*
 *  we can imagine a few different syntactic situations where we use
 *  an array
 *
 *  1) declaration and initialization
 *    a) x := [ Int x 5 ];
 *    b) x := [0, 1, 2, 3, 4, 5];
 *
 *  2) pointer access
 *    a) y := x;
 *    b) y := x + n;
 *    c) y := *(x + n);
 *    d) y := x[n];
 *
 *  3) // pointer access and indirection
 *    a) *y = 33;
 *    b) x[n] = 44;
 *    c) y := n + *y;
 *
 *  we declare an array of memory in much the same way that
 *  we declare a regular bit of memory, we simply create
 *  an alloca instruction, but we add an argument which is
 *  how many elements to allocate.
 *  in this way, the name of bound to an array alloca will act in
 *  the same way as a name bound to any other type, it will represent
 *  a pointer to the first element of the type within the allocation.
 *
 *  so, we know how to declare an array of default initialized memory,
 *  and then we can bind that declaration to a name this is what we
 *  need to do to support the semantics of 1a
 *
 *  initializing the array with specific values after allocation requires
 *  that we be able to index any particular member of the array, which
 *  means that to support 1b, we have to support the semantics of 3b,
 *  then it is simply a matter of repeating that member assignment for
 *  each member initialization. (which given that the declaration form
 *  specifies all members and their initial values means that the entire
 *  array must be indexed)
 *
 *  then, we can notice that the expression in 3b is equal to a combination of
 *  the expressions in 2b and 3a, that is
 *  x[n] = 44 is equal to *(x + n) = 44
 *
 *  looking at the llvm IR that clang emits, it seems that we can use a GEP
 *  instruction. we give the GEP instruction a pointer to the array, the
 *  array cell to index and the structure member to access, and it returns
 *  a pointer to that member of the array.
 *
 *  then we can use that pointer like we would any other pointer we already
 *  handle, that is we can indirect it to assign new contents to the memory
 *  pointed too, or we could load from it to use the memory at that location
 *
 *
 */
namespace pink {

/**
 * @brief Array represents a c-style array expression
 *
 */
class Array : public Ast {
private:
  /**
   * @brief The members of the array
   *
   */
  std::vector<std::unique_ptr<Ast>> members;

  /**
   * @brief Compute the Type of the Array expression.
   *
   * @param env the environment of this compilation unit
   * @return Outcome<Type*, Error> if true, the array type,
   * if false the Error encountered.
   */
  [[nodiscard]] auto GetypeV(const Environment &env) const
      -> Outcome<Type *, Error> override;

public:
  /**
   * @brief Construct a new Array
   *
   * @param location the textual location of this array
   * @param members the members of this array
   */
  Array(const Location &location, std::vector<std::unique_ptr<Ast>> members);

  /**
   * @brief Destroy the Array
   *
   */
  ~Array() override = default;

  Array(const Array &other) = delete;

  Array(Array &&other) = default;

  auto operator=(const Array &other) -> Array & = delete;

  auto operator=(Array &&other) -> Array & = default;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast the ast to check
   * @return true if ast *is* an instance of an Array
   * @return false if ast *is not* an instance of an Array
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief Compute the cannonical string representation of this Array
   * expression
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the llvm::ConstantArray representing this Array
   *
   * Due to a language limitation all members must be llvm::Constants
   * themselves.
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Value*, Error> if true, the llvm::ConstantArray,
   * if false, the Error encountered.
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
