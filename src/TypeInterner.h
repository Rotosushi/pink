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

  some thoughts on what this ends up
  looking like with how it is used in the
  front end.

  each member of the linked list only ever
  holds a single ProcType node. each proctype
  node could thus be held in a list of Types.
  (each initialized to the new proctype.)

*/
typedef struct TypeInterner
{
  struct Type*  nilType;
  struct Type*  integerType;
  struct Type*  booleanType;
  struct Type*  procTypes; // this one's a dynamic array.
  int           cntProcTypes; // the size of the array
} TypeInterner;

TypeInterner* CreateTypeInterner();
void          DestroyTypeInterner(TypeInterner* Ity);

// again, a lot of the benefiets of OOP
// is coming from choices I could be making
// in a C-like langauge.
// not all of them, I still can't define
// overloaded procedures or polymorphic
// procedures.

// if these procedures returned Ast terms
// which were constructed internally wrapping
// each Type* then the calling code could
// accept that any Ast term that was returned out of
// this code is somthing that needs to be free'd.
struct Type* GetNilType(TypeInterner* Ity);
struct Type* GetIntegerType(TypeInterner* Ity);
struct Type* GetBooleanType(TypeInterner* Ity);
struct Type* GetProcedureType(TypeInterner* Ity, struct Type* l, struct Type* r);


#endif // !TYPEINTERNER_H
