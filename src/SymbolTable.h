#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <stdint.h>

#include "StringInterner.h"

struct Ast;

// this is a classical hash map
// scheme, with an array of buckets
// to hash over, and each bucket is
// itself a list of entries with the
// same hash value.
typedef struct Symbol
{
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
  struct SymbolTable* enclosing_scope;
  Symbol** table;
} SymbolTable;

SymbolTable* CreateSymbolTable(SymbolTable* enclosing_scope);
SymbolTable* CloneSymbolTable(SymbolTable* other);
void DestroySymbolTable(SymbolTable* table);

struct Ast* lookup(SymbolTable* table, InternedString name);
struct Ast* lookup_in_local_scope(SymbolTable* table, InternedString name);
void bind(SymbolTable* table, InternedString name, struct Ast* term);
void unbind(SymbolTable* table, InternedString name);

#endif // !SYMBOLTABLE_H
