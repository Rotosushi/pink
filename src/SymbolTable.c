#include <stdlib.h>
#include <string>


#include "Ast.h"
#include "Utilities.hpp"
#include "StringInterner.hpp"
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

  result.success = false;

  size_t hashval = std::hash<string>{}(name);;
  hashval %= table->num_buckets;

  Symbol* cursor = table->table[hashval];

  // a matching symbol is one that meets these criteria:
  // -) the symbol shares the same interned string
  //     as the name passed in as a parameter.
  // -) the bound symbols ScopeSet is a Subset of the
  //     references Symbol's ScopeSet.
  // additionally, if we found a matching symbol already
  // we have two options, either it is a valid override of
  // the binding in which case we shall observe the the
  // binding in the higher scope will have a smaller subset
  // than the binding which was introduced in a deeper scope.
  // should the new possible match have a subset that is
  // equal in size to the subset of the current binding, we
  // are observing conflicting/ambiguous bindings.

  if (best != NULL)
  {
    ScopeSet bestsubset = Subset(best->scope, scope);
    // we already have a possible binding in best.
    if (cursor != NULL)
    {
      do
      {
        if (cursor->id == name)
        {
          ScopeSet cursorsubset = Subset(cursor->scope, scope);

          // if the subset of the cursor is larger than
          // the subset of the previous best, then
          // the cursor is the new best.
          if (cursorsubset > bestsubset)
          {
            // this symbol is the new best match
            result.success = true;
            result.term    = cursor->term;
            best           = cursor;
          }
          else if (cursorsubset < bestsubset)
          {
            // reaffirm that the result is best
            result.success = true;
            result.term    = best->term;
          }
          else
          {
            // this symbol has the same subset, which
            // can only happen if two bindings with
            // the same name are constructed in the
            // same scope.
            result.success = false;
            result.error.loc = dummy;
            result.error.dsc = appendstr("Ambiguous binding: ", name->c_str());
          }
          break;
        }

        cursor = cursor->next;
      } while (cursor != NULL);


    }
    else
    {
      // reaffirm that the result is best.
      result.success = true;
      result.term    = best->term;
    }

    // if we could keep looking, we have to?
    if (table->enclosing_table != NULL)
    {
      result = LookupSymbolHelper(table->enclosing_table, name, scope, best);
    }
    else
    {
      result.success = true;
      result.term    = best->term;
    }
  }
  else
  {
    // no current best match
    if (cursor != NULL)
    {
      // we could find a match in this bucket
      do
      {
        if (cursor->id == name)
        {
          // the name matches, we have our next best
          // match, if the scope is a subset.
          ScopeSet s = Subset(cursor->scope, scope);
          // if the Scope is a Subset then at least
          // one scope will be shared between the
          // binding and the reference.
          if (s != 0)
          {
            result.success = true;
            result.term    = cursor->term;
            best           = cursor;
          }
          else
          {
            result.success   = false;
            result.error.loc = dummy;
            result.error.dsc = appendstr("How did this happen? binding's scope is not a subset of reference's scope.", name->c_str());
          }
          break;
        }
        // walk to the next possible matching binding
        cursor = cursor->next;
      } while (cursor != NULL);

      // can we continue to look for possible better matches?
      if (table->enclosing_table != NULL)
      {
        result = LookupSymbolHelper(table->enclosing_table, name, scope, best);
      }
    }
    else
    {
      // no possible matches in this bucket.
      if (table->enclosing_table != NULL)
      {
        // search in the next outer scope.
        result = LookupSymbolHelper(table->enclosing_table, name, scope, best);
      }
      else
      {
        // this was the final scope we could be holding the binding in.
        result.success = false;
        result.error.loc = dummy;
        result.error.dsc = appendstr("Failed to find Binding: ", name->c_str());
      }
    }
  }

  return result;
}

Judgement LookupSymbol(SymbolTable* table, InternedString name, ScopeSet scope)
{
  Judgement result;
  result = LookupSymbolHelper(table, name, scope, NULL);
  return result;
}

// local lookup is exclusively used within the typechecking
// routine of 'bind' terms. where what we care about is that
// we are not duplicating a binding within the same local scope.
// if the binding exists in an outer scope, we can accept that
// this inner binding will shadow the outer binding. the name
// resolution with scope set rules will select this inner binding
// over the outer binding when references to this binding occur
// within the body of this scope. because those references will be
// constructed within the larger scope set. (only if you smuggle
// a free variable in a lambda will you be able to declare an inner
// reference with a smaller scope set that the current defining scope.)
// (this models our intuition behind 'local scopes' well.)
Judgement LookupLocalSymbol(SymbolTable* table, InternedString name)
{
  Location  dummy;
  Judgement result;
  size_t hashval = std::hash<string>{}(name);
  hashval %= table->num_buckets;

  // find the bucket to look in
  Symbol* cursor = table->table[hashval];
  // look through the bucket (list)
  if (cursor != NULL)
  {
    do {
      // does the bound name match the name we are searching for?
      if (cursor->id == name)
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
  int hashval = std::hash<string>{}(name);
  hashval %= table->num_buckets;

  // find the bucket to look in
  Symbol* head = table->table[hashval];

  Symbol* elem = (Symbol*)malloc(sizeof(Symbol));
  elem->id     = name;
  elem->scope  = scope;
  elem->term   = term;
  elem->next   = head; // prepend to the bucket, for speed.
  table->table[hashval] = elem;
  table->num_elements++;
}

void UnbindSymbol(SymbolTable* table, InternedString name)
{
  int hashval = std::hash(name);
  hashval %= table->num_buckets;

  // find the bucket to look in
  Symbol *cursor = table->table[hashval], *prev = NULL;
  // look through the bucket (list)

  // so we need to handle removing the head of the
  // list different from removing any other member
  if (name == cursor->id)
  {
    prev = cursor;
    table->table[hashval] = cursor->next;
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
