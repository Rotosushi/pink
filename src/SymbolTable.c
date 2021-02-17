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

/*
  if you generate the address of a local variable
  or member of some structure, that is always a
  non-null ptr. if the procedure gets called then
  the memory has already been allocated on the stack
  frame for the local data, and the address of that
  variable on the stack is known and not NULL.

  if you pass the address of a null pointer, we can point
  it to dynamic data via malloc.
  so this procedure can be used safely with dynamic
  memory OR stack memory, or if you are evil,
  global memory.

  though, as i think about it, we now cannot destroy the
  table safely on both assumptions (stack v. dynamic)
*/
void CloneSymbolTable(SymbolTable** destination, SymbolTable* source)
{
  if ((*destination) == NULL)
    (*destination) = (SymbolTable*)malloc(sizeof(SymbolTable));
  else
  {
    for (int i = 0; i < (*destination)->num_buckets; i++)
    {
      while ((*destination)->table[i] != NULL)
      {
        // get the first symbol in the bucket. (head)
        Symbol* cursor = (*destination)->table[i];
        // make the symbol after the first the new head
        (*destination)->table[i] = cursor->next;
        // free the previous head.
        DestroyAst(cursor->term);
        free(cursor);
      }
    }
  }

  (*destination)->num_buckets     = source->num_buckets;
  (*destination)->enclosing_scope = source->enclosing_scope;
  (*destination)->table           = (Symbol**)malloc(source->num_buckets * sizeof(Symbol*));
  for (int i = 0; i < source->num_buckets; i++)
  {
    Symbol* cursor = source->table[i];

    while (cursor != NULL)
    {
      bind((*destination), cursor->id, cursor->term);
      cursor = cursor->next;
    }
  }
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

  // so we need to handle removing the head of the
  // list different from removing any other member
  if (name == cursor->id)
  {
    prev = cursor;
    table->table[hash] = cursor->next;
    DestroyAst(prev->term);
    free(prev);
    table->num_elements--;
  }
  else
  {
    while (cursor != NULL)
    {
      prev = cursor;
      cursor = cursor->next;

      if (name == cursor->id)
      {
        // link the list around the node
        // we want to remove (cursor).
        prev->next = cursor->next;
        DestroyAst(cursor->term);
        free(cursor);
        table->num_elements--;
        break;
      }
    }
  }
}
