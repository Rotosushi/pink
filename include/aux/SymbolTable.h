/**
 * @file SymbolTable.h
 * @brief Header for class SymbolTable
 * @version 0.1
 *
 */
#pragma once
#include <optional> // std::optional
#include <utility>  // std::pair

#include "llvm/ADT/DenseMap.h" // llvm::DenseMap<...>
#include "llvm/IR/Value.h"     // llvm::Value

#include "aux/StringInterner.h" // pink::InternedString
#include "type/Type.h"          // pink::Type

namespace pink {
/**
 * @brief Manages any bindings that are requested by compilation of terms.
 *
 * A binding is equivalent to the declaration of a variable.
 *
 * \note 1/23/2023:
 *  We could optimize searching up the stack of scopes if we
 *  store all scopes related to a given compilation environment
 *  in a std::vector acting like a stack.
 *
 * \note 1/24/2023:
 *  The current implementation of the stack of scopes
 *  is to simply have the nodes of the tree own their
 *  own scope, then when we process that particular node
 *  we use it's local scope to register bindings, since
 *  we walk the tree top-down this naturally handles
 *  to insertion and deletion of scopes from the scope stack
 *  with no extra effort on the part of the program,
 *  an alternative solution would be to keep an actual stack
 *  of scopes in the environment, and have any nodes in the
 *  tree that have a local scope push a new scope onto the
 *  stack, and pop it off when finished with the task.
 *  since all scopes are stored in the same std::vector this
 *  approach may take better advantage of locality of reference
 *  when traversing up the scope stack to resolve name lookup.
 *  and given that we add and remove scopes only when necessary
 *  it may be more space efficient than the current solution which
 *  allocates all of the scopes for a given Ast for the lifetime of
 *  each Ast object.
 *
 * another benefit of this implementation is that we would no
 * longer need to construct entire Environments to represent a
 * new local environment. okay, that is a lot of object construction
 * we can avoid, this is now a todo.
 *
 *
 */
class SymbolTable {
public:
  using Key     = InternedString;
  using Value   = std::pair<Type::Pointer, llvm::Value *>;
  using Pointer = std::shared_ptr<SymbolTable>;

private:
  /**
   * @brief the mapping of all bindings within the table
   *
   */
  llvm::DenseMap<Key, Value> map;

  /**
   * @brief the outer scope of this symboltable
   *
   * if nullptr this symboltable represents global scope
   *
   * this is used during Lookup of symbols, to resolve symbols
   * defined in outer scopes.
   */
  SymbolTable *outer;

public:
  /**
   * @brief Construct a new Symbol Table object representing a global scope
   *
   */
  SymbolTable();
  /**
   * @brief Construct a new Symbol Table
   *
   * \note 1/23/2023: outer_scope == nullptr implies this scope is
   * a global scope
   *
   * @param outer
   */
  SymbolTable(SymbolTable *outer_scope);
  ~SymbolTable()                                            = default;
  SymbolTable(const SymbolTable &other)                     = default;
  SymbolTable(SymbolTable &&other)                          = default;
  auto operator=(const SymbolTable &other) -> SymbolTable & = default;
  auto operator=(SymbolTable &&other) -> SymbolTable      & = default;

  [[nodiscard]] auto OuterScope() const -> SymbolTable *;
  [[nodiscard]] auto IsGlobal() const -> bool;

  /**
   * @brief Attempts to resolve the given symbol to it's bound type and value.
   *
   * if the symbol was not within this table and this is not the global symbol
   * table then this function calls Lookup on the outer symbol table, to attempt
   * to resolve the symbol at the higher scope.
   *
   * @param symbol the symbol to resolve
   * @return llvm::Optional<std::pair<Type*, llvm::Value*>> if this option
   * contains anything, it is the resolved type and value of the symbol.
   * otherwise the symbol could not be resolved within this scope or any scope
   * this scope is within.
   */
  auto Lookup(InternedString symbol) const -> llvm::Optional<Value>;

  /**
   * @brief Attempts to resolve the given symbol to it's bound type and value.
   *
   * if the symbol was not within this table, then we simply return the empty
   * option. there is no recursive lookup performed by this function.
   *
   * that way you can query only for local variables.
   *
   * @param symbol the symbol to resolve
   * @return llvm::Optional<std::pair<Type*, llvm::Value*>> if this option
   * contains anything it is the resolved type and value of the symbol.
   * otherwise the symbol could not be resolved within the local scope.
   */
  auto LookupLocal(InternedString symbol) const -> llvm::Optional<Value>;

  /**
   * @brief Bind a given symbol within this table to the given type and value
   *
   * \warning: 9/17/2022
   * typechecking generates bindings with no corresponding value, so it is
   * there are entries in the symbol table bound to a llvm::Value of nullptr.
   * these symbols are created and then added to the 'false_bindings'
   * member of the Environment such that they can be unbound before
   * codegeneration.
   *
   * @param symbol the symbol to add to the table
   * @param type the type of the symbol (cannot be nullptr)
   * @param term the value of the symbol (can be nullptr)
   */
  void Bind(Key symbol, Type::Pointer type, llvm::Value *term);

  /**
   * @brief Unbind a given symbol within this table.
   *
   * @param symbol the symbol to remove from the table
   */
  void Unbind(Key symbol);
};
} // namespace pink
