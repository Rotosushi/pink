#include <stdlib.h>

#include "Ast.h"
#include "StringInterner.h"
#include "SymbolTable.h"

#define SYMTABLE_DEFAULT_NUM_BUCKETS 10

SymbolTable* CreateSymbolTable(SymbolTable* enclosing_table)
{
  SymbolTable* result     = (SymbolTable*)malloc(sizeof(SymbolTable));
  result->num_buckets     = SYMTABLE_DEFAULT_NUM_BUCKETS;
  result->num_elements    = 0;
  result->enclosing_table = enclosing_table;
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
  (*destination)->enclosing_table = source->enclosing_table;
  (*destination)->table           = (Symbol**)malloc(source->num_buckets * sizeof(Symbol*));
  for (int i = 0; i < source->num_buckets; i++)
  {
    Symbol* cursor = source->table[i];

    while (cursor != NULL)
    {
      BindSymbol((*destination), cursor->id, cursor->scope, cursor->term);
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



Judgement LookupSymbolHelper(SymbolTable* table, InternedString name, ScopeSet scope, Symbol* best)
{
  Location  dummy;
  Judgement result;
  int hash = (int)(name);
  hash %= table->num_buckets;

  Symbol* cursor = table->table[hash];

  if (cursor != NULL)
  {
    do {
      // does the bound name match the name we are searching for?
      if (cursor->name == name)
      {
        // if the bound identifier's Scope is a subset of
        // the ScopeSet we are looking in, then we have found
        // a possible binding.
        ScopeSet s0 = Subset(cursor->scope, scope);

        if (s0 && best == NULL)
        {
          best = cursor;
          break;
        }
        else
        {
          // we already found a candidate match for this binding,
          // but are currently looking for more possible bindings.
          // should a binding we find have a larger subset than
          // the current best match, the binding we found becomes
          // the new best match.
          ScopeSet s1 = Subset(cursor->scope, best->scope);
          if (s0 == s1)
          {
            result.success   = false;
            result.error.loc = dummy;
            result.error.dsc = dupstr("Conflicting Bindings!");
          }
          else if (s0 > s1)
          {
            best = cursor;
            result.success = true;
            result.term    = best->term;
          }
          break;
        }
      }
      cursor = cursor->next;
    } while (cursor != NULL);

    if (result.success == true)
    {
      return LookupSymbolHelper(table->enclosing_table, name, scope, best);
    }
    else
    {
      return result;
    }
  }
  else
  {
    result.success = false;
    result.error.loc = dummy;
    result.error.dsc = dupstr("Failed to Find Binding!");
    return result;
  }

}

Judgement LookupSymbol(SymbolTable* table, InternedString name, ScopeSet scope)
{
  Judgement result;
  result = LookupSymbolHelper(table, name, scope, NULL);
  return result;
}

Judgement LookupLocal(SymbolTable* table, InternedString name)
{
  Location  dummy;
  Judgement result;
  int hash = (int)(name);
  hash %= table->num_buckets;

  // find the bucket to look in
  Symbol* cursor = table->table[hash];
  // look through the bucket (list)
  if (cursor != NULL)
  {
    do {
      // does the bound name match the name we are searching for?
      if (cursor->name == name)
      {
        result.success = true;
        result.term    = cursor->term;
      }
      cursor = cursor->next;
    } while (cursor != NULL);

    if (cursor == NULL)
    {
      result.success = false;
      result.error.loc = dummy;
      result.error.dsc = dupstr("Failed to find Binding");
    }
  }
  else
  {
    result.success = false;
    result.error.loc = dummy;
    result.error.dsc = dupstr("Failed to find Binding");
  }
  return result;
}

void BindSymbol(SymbolTable* table, InternedString name, ScopeSet scope, Ast* term)
{
  int hash = (int)(name);
  hash %= table->num_buckets;

  // find the bucket to look in
  Symbol* head = table->table[hash];

  Symbol* elem = (Symbol*)malloc(sizeof(Symbol));
  elem->id     = name;
  elem->scope  = scope;
  elem->term   = term;
  elem->next   = head; // prepend to the bucket, for speed.
  table->table[hash] = elem;
  table->num_elements++;
}

void UnbindSymbol(SymbolTable* table, InternedString name)
{
  int hash = (int)(name);
  hash %= table->num_buckets;

  // find the bucket to look in
  Symbol *cursor = table->table[hash], *prev = NULL;
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
