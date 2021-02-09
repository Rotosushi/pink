#ifndef AST_H
#define AST_H

// Ast sits at the bottom of an
// upside-down information pyramid
// however, notice that in OOP,
// it is considered the
// TOP of the heirarchy.


#include "Location.h"
#include "Variable.h"
#include "Application.h"
#include "Assignment.h"
#include "Bind.h"
#include "Binop.h"
#include "Unop.h"
#include "Conditional.h"
#include "Iteration.h"
#include "Type.h"
#include "Object.h"
#include "TypeJudgement.h"

struct Environment;
struct TypeInterner;

typedef enum AstKind
{
  A_VAR,
  A_APP,
  A_ASS,
  A_BND,
  A_BOP,
  A_UOP,
  A_CND,
  A_ITR,
  A_OBJ,
  A_TYP,
} AstKind;

// the kinds of Ast represent the
// parts of symbolic computation
// supported by Pink.
typedef struct Ast
{
  AstKind kind;
  Location loc;
  union {
    // nothing is storing anthing
    // except pointers, so, modulus
    // padding, ast structs are about
    // sizeof(enum AstKind)
    // + sizeof(4* sizeof(int)) // sizeof(Location)
    // + sizeof(Object) // the largest member
    // big.
    // Object is the largest member because
    // sizeof(Lambda) is about 4 pointers.
    // every other construct holds less than
    // 4 pointers.
    Variable    var;
    Application app;
    Assignment  ass;
    Bind        bnd;
    Binop       bop;
    Unop        uop;
    Conditional cnd;
    Iteration   itr;
    Object      obj;
    Type       *typ; // ewwwwww...
  };
} Ast;

// now, you may say, hey! you are now
// storing way more memory per node than
// the absolute bare minimum. like,
// if i want a tree that represents an Integer.
// that takes up just as much space as a tree
// that represents a lambda. that is super
// inneffecient. and, yep. it totally is.
// however, think of it this way, now,
// when the computer is trying to process a node,
// figure out what kind of node it is, or compare the
// types of nodes, we already have the pointers we
// need, pulled into memory by the first access
// of the particular node in the tree. meaning
// each of the sub procedures that get called
// are working with memory that is already hot
// in the cache! that is way faster than potentially
// hitting a cache miss because the runtime
// allocated the memory representing the innards of
// the node in another page of memory. something
// we have no control over when it happens.
// so, in spite of the fact that we are allocating more
// memory per node, we are going to run faster (HYPOTHESIS),
// because the data we want is always directly stored
// in a larger chunk of memory.
// additionally, the disadvantage of each node being
// slightly more expensive is lessened by the fact
// that we are working with significantly more
// memory than ever was imagined by early
// computer scientists.

Ast* CreateVariable(InternedString id);
Ast* CreateApplication(Ast* left, Ast* right);
Ast* CreateAssignment(Ast* left, Ast* right);
Ast* CreateBind(InternedString id, Ast* right);
Ast* CreateBinop(InternedString op, Ast* left, Ast* right);
Ast* CreateUnop(InternedString op, Ast* right);
Ast* CreateConditional(Ast* condition, Ast* first_alternative, Ast* second_alternative);
Ast* CreateIteration(Ast* condition, Ast* body);
Ast* CreateNil();
Ast* CreateInteger(int  value);
Ast* CreateBoolean(bool value);
Ast* CreateLambda(InternedString arg_id, Type* arg_type, Ast* body);
Ast* CreateType(Type* type);
Ast* CreateScalarType(ScalarKind kind, struct TypeInterner* interned_types);
Ast* CreateProcType(Type* left, Type* right, struct TypeInterner* interned_types);

// this shall deallocate all memory associated
// with the ast.
void DestroyAst(Ast* ast);

// constructs a new tree exaclty the same as the
// passed tree.
void CloneAst(Ast** destination, Ast* source);

// constructs a string that represents the passed
// in tree.
char* ToStringAst(Ast* ast);

// returns the type of the tree passed in,
// or a description of the error that was found.
TypeJudgement Getype(Ast* ast, struct Environment* env);

// EvalJudgement Evaluate(Ast* ast, Environment* env);

#endif // !AST_H
