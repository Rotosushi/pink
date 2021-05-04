#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <stdint.h>

#include "Judgement.h"
#include "ScopeSet.h"
#include "StringInterner.hpp"

struct Ast;

// this is a classical hash map
// scheme, with an array of buckets
// to hash over, and each bucket is
// itself a list of entries with the
// same hash value.
typedef struct Symbol
{
  // we could describe the symbol table as binding
  // (identifier, scope) pairs to terms.
  ScopeSet       scope;
  InternedString id;
  struct Ast*    term;
  struct Symbol* next;
} Symbol;

typedef struct SymbolTable
{
  // load factor is [num_elements \ num_buckets]
  // when the load factor becomes large, this
  // is the signal to increase the size of the
  // table. the particular value which signals
  // a resize will require testing however.
  int num_buckets;
  int num_elements;
  struct SymbolTable* enclosing_table;
  Symbol** table;
} SymbolTable;

SymbolTable* CreateSymbolTable(SymbolTable* enclosing_table);
void CloneSymbolTable(SymbolTable **destination, SymbolTable* source);
void DestroySymbolTable(SymbolTable* table);

Judgement LookupSymbol(SymbolTable* table, InternedString name, ScopeSet scope);
Judgement LookupLocalSymbol(SymbolTable* table, InternedString name);
void BindSymbol(SymbolTable* table, InternedString name, ScopeSet scope, struct Ast* term);
void UnbindSymbol(SymbolTable* table, InternedString name);

#endif // !SYMBOLTABLE_H
