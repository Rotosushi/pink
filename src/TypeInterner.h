#ifndef TYPEINTERNER_H
#define TYPEINTERNER_H

struct Ast;
struct Type;

typedef struct TLelem
{
  struct Type*   type;
  struct TLelem* next;
} TLelem;

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
  array access. the type that -must-
  to be interned as a dynamic list
  of types is exclusively ProcType
  given the current langauge support.
  though i suspect structures and unions
  will similarly  need to be stored as a
  set of known definitions according to
  some internal pattern,

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
  TLelem*       procTypes;
} TypeInterner;

TypeInterner* CreateTypeInterner();
void          DestroyTypeInterner(TypeInterner* Ity);

// again, a lot of the benefiets of OOP
// is coming from choices I could be making
// in a C-like langauge.
// not all of them, I still can't define
// overloaded procedures or polymorphic
// procedures.
struct Type* GetNilType(TypeInterner* Ity);
struct Type* GetIntegerType(TypeInterner* Ity);
struct Type* GetBooleanType(TypeInterner* Ity);
struct Type* GetProcedureType(TypeInterner* Ity, struct Ast* l, struct Ast* r);


#endif // !TYPEINTERNER_H
