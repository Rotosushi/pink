#ifndef TYPEINTERNER_H
#define TYPEINTERNER_H

struct Type;


/*
  okay, so a super basic version of
  this data structure could behave
  nearly identically to the symbol
  table, except instead of strcmp
  we use EqualTypes. this however
  would be very inefficient. we
  have a type schema that includes
  a lot of basic primitive types,
  which could be stored and accessd
  directly, skipping any sort of
  array access. the type that needs
  to be interned as a dynamic list
  of types is exclusively ProcType
  given the current langauge support.

  also also, this can be refactored
  into an LLVM interface at some later
  date. while the langauge can take
  advantage of the encoding style rn.
*/
typedef struct TypeInterner
{
  struct Type*  nilType;
  struct Type*  integerType;
  struct Type*  booleanType;
  struct Type** procTypes;
  int    procTypesLength;
} TypeInterner;

TypeInterner* CreateTypeInterner();
void          DestroyTypeInterner(TypeInterner* Ity);

struct Type* GetNilType(TypeInterner* Ity);
struct Type* GetIntegerType(TypeInterner* Ity);
struct Type* GetBooleanType(TypeInterner* Ity);
struct Type* GetProcedureType(TypeInterner* Ity, struct Type* l, struct Type* r);


#endif // !TYPEINTERNER_H
