#include <stdlib.h>

#include "Ast.h"
#include "StringInterner.h"
#include "SymbolTable.h"

#define SYMTABLE_DEFAULT_NUM_BUCKETS 10

SymbolTable* CreateSymbolTable(SymbolTable* enclosing_scope)
{
  SymbolTable* result     = (SymbolTable*)malloc(sizeof(SymbolTable));
  result->num_buckets     = SYMTABLE_DEFAULT_NUM_BUCKETS;
  result->num_elements    = 0;
  result->enclosing_scope = enclosing_scope;
  result->table           = (Symbol**)malloc(result->num_buckets * sizeof(Symbol*));
  for (int i = 0; i < result->num_buckets; i++)
  {
    result->table[i] = NULL;
  }
  return result;
}

SymbolTable* CloneSymbolTable(SymbolTable* other)
{
  SymbolTable* result     = (SymbolTable*)malloc(sizeof(SymbolTable));
  result->num_buckets     = other->num_buckets;
  result->enclosing_scope = other->enclosing_scope;
  result->table           = (Symbol**)malloc(other->num_buckets * sizeof(Symbol*));
  for (int i = 0; i < other->num_buckets; i++)
  {
    Symbol* cursor = other->table[i];

    while (cursor != NULL)
    {
      bind(result, cursor->id, cursor->term);
      cursor = cursor->next;
    }
  }
  return result;
}

void DestroySymbolTable(SymbolTable* table)
{
  if (!table) return;

  for (int i = 0; i < table->num_buckets; i++)
  {
    while (table->table[i] != NULL)
    {
      // get the first symbol in the bucket. (head)
      Symbol* cursor = table->table[i];
      // make the symbol after the first the new head
      table->table[i] = cursor->next;
      // free the previous head.
      DestroyAst(cursor->term);
      free(cursor);
    }
  }
  free(table->table);
  free(table);
}

Ast* lookup(SymbolTable* table, InternedString name)
{
  int hash = (int)(name);
  hash %= table->num_buckets;

  // find the bucket to look in
  Symbol* cursor = table->table[hash];
  // look through the bucket (list)
  while (cursor != NULL)
  {
    // takin' advantage of interned strings here
    if (name == cursor->id)
      return cursor->term;

    cursor = cursor->next;
  }
  // search the enclosing scope for the name
  if (table->enclosing_scope != NULL)
    return lookup(table->enclosing_scope, name);
  else
    return (Ast*)NULL;
}

Ast* lookup_in_local_scope(SymbolTable* table, InternedString name)
{
  int hash = (int)(name);
  hash %= table->num_buckets;

  // find the bucket to look in
  Symbol* cursor = table->table[hash];
  // look through the bucket (list)
  while (cursor != NULL)
  {
    if (name == cursor->id)
      return cursor->term;

    cursor = cursor->next;
  }
  return (Ast*)NULL;
}

void bind(SymbolTable* table, InternedString name, Ast* term)
{
  int hash = (int)(name);
  hash %= table->num_buckets;

  // find the bucket to look in
  Symbol* head = table->table[hash];

  Symbol* elem = (Symbol*)malloc(sizeof(Symbol));
  elem->id     = name;
  elem->term   = term;
  elem->next   = head; // prepend to the bucket, for speed.
  table->table[hash] = elem;
  table->num_elements++;
}

void unbind(SymbolTable* table, InternedString name)
{
  int hash = (int)(name);
  hash %= table->num_buckets;

  // find the bucket to look in
  Symbol* cursor = table->table[hash], *prev = NULL;
  // look through the bucket (list)
  while (cursor != NULL)
  {
    if (name == cursor->id)
    {
      prev->next = cursor->next;
      DestroyAst(cursor->term);
      free(cursor);
      table->num_elements--;
      break;
    }

    prev   = cursor;
    cursor = cursor->next;
  }
}
