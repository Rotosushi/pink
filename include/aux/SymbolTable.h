/**
 * @file SymbolTable.h
 * @brief Header for class SymbolTable
 * @version 0.1
 *
 */
#pragma once
#include <utility> // std::pair<>

#include "llvm/ADT/APInt.h"    // llvm::Optional<>
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
 */
class SymbolTable {
private:
  /**
   * @brief the mapping of all bindings within the table
   *
   */
  llvm::DenseMap<InternedString, std::pair<Type *, llvm::Value *>> map;

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
   * @brief Construct a new Symbol Table.
   *
   */
  SymbolTable();

  /**
   * @brief We never want to construct a new symbol table as a copy of another
   * symbol table
   *
   * @param other the symbol table to copy
   */
  SymbolTable(const SymbolTable &other) = delete;

  /**
   * @brief Construct a new local Symbol Table.
   *
   * @param outer the scope that this Symbol Table resides within.
   */
  SymbolTable(SymbolTable *outer);

  /**
   * @brief Destroy the Symbol Table
   *
   */
  ~SymbolTable() = default;

  /**
   * @brief getter for the outer scope of this symbol table
   *
   * @return SymbolTable*
   */
  [[nodiscard]] auto OuterScope() const -> SymbolTable *;

  /**
   * @brief returns whether this is a global scope symbol table
   *
   * @return true if this *is* the global symbol table
   * @return false if this *is not* the global symbol table
   */
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
  auto Lookup(InternedString symbol) const
      -> llvm::Optional<std::pair<Type *, llvm::Value *>>;

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
  auto LookupLocal(InternedString symbol) const
      -> llvm::Optional<std::pair<Type *, llvm::Value *>>;

  /**
   * @brief Bind a given symbol within this table to the given type and value
   *
   * Note: 9/17/2022
   * typechecking generates bindings with no corresponding value, so it is
   * not unheard of to find a symbol bound to a value of nullptr.
   * these symbols are created and then added to the 'false_bindings'
   * member of the Environment such that they can be unbound before
   * codegeneration.
   *
   * \warning the symboltable itself does nothing to prevent the creation
   * of duplicate symbols. nor does it check that type or value are not
   * nullptrs.
   *
   * @param symbol the symbol to add to the table
   * @param type the type of the symbol
   * @param term the value of the symbol
   */
  void Bind(InternedString symbol, Type *type, llvm::Value *term);

  /**
   * @brief Unbind a given symbol within this table.
   *
   * @param symbol the symbol to remove from the table
   */
  void Unbind(InternedString symbol);
};
} // namespace pink
