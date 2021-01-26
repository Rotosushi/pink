#ifndef AST_H
#define AST_H

// Ast sits at the bottom of an
// upside-down information pyramid
// however, notice that in OOP,
// it is considered the
// TOP of the heirarchy.


#include "StringLocation.h"
#include "Environment.h"
#include "Variable.h"
#include "Application.h"
#include "Assignment.h"
#include "Bind.h"
#include "Binop.h"
#include "Unop.h"
#include "Conditional.h"
#include "Iteration.h"
#include "Object.h"
#include "TypeJudgement.h"

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
} AstKind;

// the kinds of Ast represent the
// parts of symbolic computation
// supported by Pink.
typedef struct Ast
{
  AstKind kind;
  StringLocation loc;
  union {
    Variable    *var;
    Application *app;
    Assignment  *ass;
    Bind        *bnd;
    Binop       *bop;
    Unop        *uop;
    Conditional *cnd;
    Iteration   *itr;
    Object      *obj;
  };
} Ast;

// this shall deallocate all memory associated
// with term, such that after this procedure
// returns term is null.
void DestroyAst(Ast* term);

// constructs a new tree exaclty the same as the
// passed tree.
Ast* CloneAst(Ast* term);

// constructs a string that represents the passed
// in term.
char* ToStringAst(Ast* term);

// returns the type of the term passed in,
// or a description of the error that was found.
TypeJudgement getype(Ast* term, Environment* env);

// EvalJudgement evaluate(Ast* term, Environment* env);

#endif // !AST_H
