/**
 * @file Block.h
 * @brief Header for class Block
 * @version 0.1
 */
#pragma once
#include <vector>

#include "aux/SymbolTable.h"

#include "ast/Ast.h"

namespace pink {
/**
 * @brief Represents a Block of expressions
 *
 */
class Block : public Ast {
private:
  /**
   * @brief Compute the Type of the block expression
   *
   * the result type of a block is the type of the last statement in
   * the sequence.
   *
   * @param env the environment of this compilation unit
   * @return Outcome<Type*, Error> if true the return type of the block
   * expression, if false the Error encountered.
   */
  [[nodiscard]] auto TypecheckV(const Environment &env) const
      -> Outcome<Type *, Error> override;

public:
  /**
   * @brief the sequence of statements composing the block
   *
   */
  std::vector<std::unique_ptr<Ast>> statements;

  /**
   * @brief Construct a new empty Block
   *
   * @param location the textual location of the block
   */
  Block(const Location &location);

  /**
   * @brief Construct a new Block
   *
   * @param location the textual location of the block
   * @param statements the statements composing the block
   */
  Block(const Location &location,
        std::vector<std::unique_ptr<Ast>> &statements);

  /**
   * @brief Destroy the Block
   *
   */
  ~Block() override = default;

  Block(const Block &other) = delete;

  Block(Block &&other) = default;

  auto operator=(const Block &other) -> Block & = delete;

  auto operator=(Block &&other) -> Block & = default;

  /**
   * @brief part of the Visitor interface
   *
   * @param visitor the visitor to accept
   */
  void Accept(const ConstAstVisitor *visitor) const override;

  /**
   * @brief an iterator over the statements within the block
   *
   */
  using iterator = std::vector<std::unique_ptr<Ast>>::iterator;
  using const_iterator = std::vector<std::unique_ptr<Ast>>::const_iterator;

  /**
   * @brief Get the iterator to the beginning of the block
   *
   * @return iterator the iterator to the beginning of the block
   */
  [[nodiscard]] auto begin() -> iterator { return statements.begin(); }
  [[nodiscard]] auto cbegin() const -> const_iterator {
    return statements.cbegin();
  }

  /**
   * @brief Get the iterator to the end of the block
   *
   * @return iterator the iterator to the end of the block
   */
  [[nodiscard]] auto end() -> iterator { return statements.end(); }
  [[nodiscard]] auto cend() const -> const_iterator {
    return statements.cend();
  }

  /**
   * @brief Implements LLVM style [RTTI] for this class
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   * @param ast the ast to test
   * @return true if ast *is* an instance of a Block
   * @return false if ast *is not* an instance of a Block
   */
  static auto classof(const Ast *ast) -> bool;

  /**
   * @brief Return the cannonical string representation of the Block
   *
   * @return std::string the string representation
   */
  [[nodiscard]] auto ToString() const -> std::string override;

  /**
   * @brief Compute the Value of this Block
   *
   * The result Value of a Block is the result value of the last statement
   * within the Block
   *
   * @param env the environment of this compilation unit
   * @return Outcome<llvm::Value*, Error> if true the result value of the Block,
   * if false the Error encountered.
   */
  [[nodiscard]] auto Codegen(const Environment &env) const
      -> Outcome<llvm::Value *, Error> override;
};
} // namespace pink
