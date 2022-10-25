/**
 * @file Binop.h
 * @brief Header for class Binop
 * @version 0.1
 */
#pragma once
#include <string>

#include "ast/Ast.h"

#include "ops/BinopTable.h"

#include "aux/StringInterner.h"

#include "type/ArrayType.h"
#include "type/IntType.h"
#include "type/SliceType.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"

namespace pink {
/**
 * @brief Represents an instance of a binary operator expression
 *
 */
class Binop : public Ast {
private:
  /*
    auto TypecheckLHSArrayAdd(ArrayType *splice_type, Type *rhs_type,
                              const Environment &env) const
        -> Outcome<Type *, Error>;

    auto TypecheckRHSArrayAdd(ArrayType *array_type, Type *lhs_type,
                              const Environment &env) const
        -> Outcome<Type *, Error>;

    auto CodegenGlobalArrayAdd(llvm::StructType *array_splice_type,
                               llvm::Value *array_splice_ptr,
                               llvm::ConstantInt *index_ptr,
                               const Environment &env) const
        -> Outcome<llvm::Value *, Error>;

    auto CodegenLocalArrayAdd(llvm::StructType *array_splice_type,
                              llvm::Value *array_splice_ptr,
                              llvm::ConstantInt *index_ptr,
                              const Environment &env) const
        -> Outcome<llvm::Value *, Error>;

    auto CodegenLocalSliceAdd(SliceType *slice_type, llvm::Value *slice_ptr,
                              llvm::ConstantInt *index_ptr,
                              const Environment &env) const
        -> Outcome<llvm::Value *, Error>;
    */
  /**
   * @brief Compute the Type of this Binop expression
   *
   * the type of a binop expression is the return type after applying the binop.
   *
   * @param env the environment of this compilation unit
   * @return Outcome<Type*, Error> if true the type of the binop expression,
   * if false the Error encountered.
   */
  [[nodiscard]] auto TypecheckV(const Environment &env) const
      -> Outcome<Type *, Error> override;

  /**
   * @brief The binary operator of this expression
   *
   */
  InternedString op;

  /**
   * @brief the left hand side of the binop expression
   *
   */
  std::unique_ptr<Ast> left;

  /**
   * @brief the right hand side of the binop expression
   *
   */
  std::unique_ptr<Ast> right;

public:
  /**
   * @brief Construct a new Binop
   *
   * @param location the textual location of this binop expression
   * @param opr the operator of the binop expression
   * @param left the left hand side of the binop expression
   * @param right the right hand side of the binop expression
   */
  Binop(const Location &location, InternedString opr, std::unique_ptr<Ast> left,
        std::unique_ptr<Ast> right);

  /**
   * @brief Destroy the Binop
   *
   */
  ~Binop() override = default;

  Binop(const Binop &other) = delete;

  Binop(Binop &&other) = default;

  auto operator=(const Binop &other) -> Binop & = delete;

  auto operator=(Binop &&other) -> Binop & = default;

  auto GetOp() const -> InternedString { return op; }

  auto GetLeft() const -> const Ast * { return left.get(); }

  auto GetRight() const -> const Ast * { return right.get(); }
  /**
   * @brief part of the Visitor interface
   *
   * @param visitor the visitor to accept
   */
  void Accept(AstVisitor *visitor) const override;

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast the ast to test
   * @return true if ast *is* an instance of a Binop
   * @return false if ast *is not* an instance of a Binop
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief Compute the cannonical string representation of the Binop expression
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the Value of this Binop expression
   *
   * the value is the result value after applying the binop to the
   * value of the left and right sides.
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Value*, Error> if true the value after applying the
   * binop, if false the Error encountered.
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
