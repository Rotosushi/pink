/**
 * @file SymbolTable.h
 * @brief Header for class SymbolTable
 * @version 0.1
 *
 */
#pragma once
#include <optional> // std::optional
#include <tuple>    // std::tuple
#include <vector>   // std::vector

#include "llvm/IR/Value.h" // llvm::Value

#include "aux/StringInterner.h" // pink::InternedString
#include "type/Type.h"          // pink::Type

namespace pink {
/**
 * @brief Manages all bindings for a single scope.
 */
class SymbolTable {
public:
  using Value = std::tuple<InternedString, Type::Pointer, llvm::Value *>;

private:
  static constexpr auto initial_size = 5;

  std::vector<Value> table;
  SymbolTable       *outer_scope;

public:
  SymbolTable()
      : table{initial_size},
        outer_scope{nullptr} {}
  SymbolTable(SymbolTable *outer_scope)
      : table{initial_size},
        outer_scope{outer_scope} {}
  ~SymbolTable()                                            = default;
  SymbolTable(const SymbolTable &other)                     = default;
  SymbolTable(SymbolTable &&other)                          = default;
  auto operator=(const SymbolTable &other) -> SymbolTable & = default;
  auto operator=(SymbolTable &&other) -> SymbolTable      & = default;

  [[nodiscard]] auto OuterScope() const -> SymbolTable * { return outer_scope; }

  [[nodiscard]] auto IsGlobal() const -> bool { return outer_scope == nullptr; }

  /**
   * @brief Attempts to resolve the given symbol to it's bound type and value.
   *
   * if the symbol was not within this table and this is not the global symbol
   * table then this function calls Lookup on the outer_scope symbol table, to
   * attempt to resolve the symbol at the higher scope.
   *
   * \note how weird is it to return a copy of the binding here?
   * it is three pointers, so that's not really the issue. and
   * it is by value, so the caller cannot modify the table by
   * assigning to the returned value. So it seems fine?
   *
   * @param symbol the symbol to resolve
   * @return llvm::Optional<std::pair<Type*, llvm::Value*>>
   */
  auto Lookup(InternedString symbol) const -> std::optional<Value>;

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
  auto LookupLocal(InternedString symbol) const -> std::optional<Value>;

  /**
   * @brief Bind a given symbol within this table to the given type and value
   *
   * @param symbol the symbol to add to the table
   * @param type the type of the symbol (cannot be nullptr)
   * @param term the value of the symbol (can be nullptr)
   */
  void Bind(InternedString symbol, Type::Pointer type, llvm::Value *term) {
    table.emplace_back(symbol, type, term);
  }
};
} // namespace pink
