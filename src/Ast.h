#ifndef AST_H
#define AST_H

/*

typedef struct Ast
{
  AstKind kind;
  Location loc; // if we shift the Location struct into the member
                // objects then
                // A) the size of the Ast struct doesn't change,
                //    each member only has one location, same as before.
                //    proof by induction on c-style union storage rules
                // B) we no longer need to have the type of Getype*
                //    procedures be uniformly Ast*, instead we can
                //    truly pass in a "inner" type pointer, which
                //    allows a language like mine, with procedures
                //    being overloadable on argument type, with
                //    runtime dispatching to define a procedure which
                //    is overloaded over every member type, and the
                //    parent union type to correctly associate a
                //    body with a member of each type.
                //    the call graph could look like:
                //    we call the Ast* getype which simply dispatches to
                //    each member types getype depending upon which type
                //    is live within the union at the time of calling?
                //    hmmm....
                //    does the programmer even get to define the Ast* typed
                //    procedure if we want the compiler to infer the
                //    dispatch procedure, freeing us from having to remember
                //    to check every case because the compiler can know
                //    if we miss to handle one.
  union {
    // nothing is storing anthing
    // except pointers, so, modulus
    // padding, ast structs are about
    // sizeof(enum AstKind)
    // + sizeof(4* sizeof(int)) // sizeof(Location)
    // + sizeof(Object) // the largest member
    // big.
    // Object is the largest member because
    // it holds another enum for it's union, and
    // sizeof(Lambda) is about 4 pointers.
    // every other construct holds less than
    // 4 pointers.
    Variable    var; // {char*}
    Application app; // {Ast*, Ast*}
    Assignment  ass; // {Ast*, Ast*}
    Bind        bnd; // {char*, Ast*}
    Binop       bop; // {char*, Ast*, Ast*}
    Unop        uop; // {char*, Ast*}
    Conditional cnd; // {Ast*, Ast*, Ast*}
    Iteration   itr; // {Ast*, Ast*}
    Object      obj; // {enum ObjKind, union:{ nil      // {void*}
                                             , integer  // {int}
                                             , boolean  // {bool}
                                             , lambda   // {char*, Type*, Ast*, SymbolTable*}
                                            }}
    Type       *typ; // ewwwwww... (wait no, thanks to interning, pointers -are- the type literal)
  };
} Ast;
*/
// Ast sits at the bottom of an
// upside-down information pyramid
// however, notice that in OOP,
// it is considered the
// TOP of the heirarchy.
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
// (and given a program running long enough using
// some large (n) amount of memory, we would see
// some nodes which would invoke this behavior.)
// so, in spite of the fact that we are allocating more
// memory per node, we are going to run faster (HYPOTHESIS),
// because the data we want is always directly stored
// in a larger chunk of memory.
// additionally, the disadvantage of each node being
// slightly more expensive is lessened by the fact
// that we are working with significantly more
// memory than ever was imagined by early
// computer scientists. gigabytes in a desktop/server
// environment, more than the kilobytes that used to be
// considered large. I would
// like this langauge to write programs that can fit
// in 1kb of storage, with less working memory.
// and we can grow that sort of program when we have the
// resources to add more

// TL;DR:
// ast nodes used to look like
// ast-union -> member-data
// where we have a pointer between
// the union of types and the actual
// data being stored.
// which was repeated in the case of
// Objects, which themselves are another
// union of types. this means that to
// store an integer literal took
// two pointers!
// ast-union -> object-union -> integer-literal
// this is indirection that needs to
// be traversed before any useful
// work can be done with the value.
// Now, instead of this, both ast and object
// have been refactored such that the union is
// over the member objects directly, instead of
// pointers-to-member-objects. (which could have
// just been a single void* without much change.)
// anyways, this means that instead of any indirection
// we have all of the data stored directly within the
// ast-union memory. (this is still a tree structure
// though, so, mostly pointers anyways, especially now
// that identifiers, operators, and types are interned.)


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
#include "TypeLiteral.h"
#include "Object.h"
#include "Judgement.h"
#include "Judgement.h"

struct SymbolTable;
struct Environment;
struct TypeInterner;

/*
  Ast is the type that can take on many forms at runtime,
  these forms are described elsewhere, but we know which
  forms are being referenced, so if we can know the definition
  of the form at some later time this structure can have it's
  memory layout generated by the compiler.

*/

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
  union {
    Variable    var;
    Application app;
    Assignment  ass;
    Bind        bnd;
    Binop       bop;
    Unop        uop;
    Conditional cnd;
    Iteration   itr;
    Object      obj;
  };
} Ast;


// these cannot be represented as simply a
// overload set, as Application, Assignment and Iteration
// share the exact same parameter lists.
// so do, CreateAstBind and CreateAstBinop
// this might say something towards what semantics
// we can give a type system built directly upon
// C semantics. maybe we supply the constructor
// procedure with the typename to give something
// to switch over? OOP uses a Type record with a
// pointer to the proper constructor procedure.
// one of which is a vararg procedure.
Ast* CreateAstVariable(InternedString id, ScopeSet scope, Location* loc);
Ast* CreateAstApplication(Ast* left, Ast* right, int num_args, Location* loc);
Ast* CreateAstAssignment(Ast* left, Ast* right, Location* loc);
Ast* CreateAstBind(InternedString id, Ast* right, Location* loc);
Ast* CreateAstBinop(InternedString op, Ast* left, Ast* right, Location* loc);
Ast* CreateAstUnop(InternedString op, Ast* right, Location* loc);
Ast* CreateAstConditional(Ast* condition, Ast* first, Ast* second, Location* loc);
Ast* CreateAstIteration(Ast* condition, Ast* body, Location* loc);
Ast* CreateAstNil(Location* loc);
Ast* CreateAstInteger(int  value, Location* loc);
Ast* CreateAstBoolean(bool value, Location* loc);
Ast* CreateAstLambda(InternedString arg_id, Ast* arg_type, Ast* body, struct SymbolTable* symbols, ScopeSet scope, Location* loc);
Ast* CreateAstPartiallyAppliedLambda(Ast* rest, InternedString arg_name, ScopeSet arg_scope, Ast* arg_value, Ast* arg_type, Location* loc);
Ast* CreateAstType(Type* type, Location* loc);



Judgement Assign(Ast* dest, Ast* source);

Judgement Equals(Ast* a1, Ast* a2);

// returns a reference to the location data
// held within the member of the ast.
// switching based upon the active member.
Location* GetAstLocation(Ast* ast);

Type* GetTypePtrFromLiteral(Ast* tl);

// this shall deallocate all memory associated
// with the ast.
void DestroyAst(Ast* ast);

// constructs a new tree exaclty the same as the
// passed tree.
Ast* CloneAst(Ast** destination, Ast* source);

// constructs a string that represents the passed
// in tree. the caller is responsible for managing
// the string passed back.
char* ToStringAst(Ast* ast);

// getype builds a tree representing the type of
// the tree that was passed in.
Judgement Getype(Ast* ast, struct Environment* env);

// evaluate builds a tree represnting the result of
// evaluating the passed in tree.
Judgement Evaluate(Ast* ast, struct Environment* env);


// AppearsFree, RenameBinding, and Substitute
// are all used for Applying Lambdas, and as such
// are only ever used on copies of Lambdas.

// returns true if id can be found free
// when searching the term.
bool AppearsFree(Ast* term, InternedString id);

// replaces all free instances of target with replacement,
// if replacement is NULL, we create a new id, intern it
// and use that as the replacement. new id's are of the
// form, p0, p1, p2, ..., pn where the number is incremented
// with each new id we generate.
// this means we never generate
// a new id which conflicts with a previously generated id.
// although, to be fully consistent, i think calls of AppearsFree
// will be required of generated names within the target term.
// to be completely sure we aren't creating an error.
// although, it would be in a case where the programmer
// has named a variable p0 or p1 or so forth.
void RenameBinding(Ast* term, InternedString target, InternedString replacement);

// this performs a substitution upon the tree pointed to
// by term. each instance of a variable leaf node containing
// the matching id will be replaced by the -same- Ast ptr.
// this may or may not cause weird bugs, i accept whatever
// semantics arise from this, and am merely content to observe
// them while i learn the lay of the land.
void Substitute(Ast* term, Ast** target, InternedString id, Ast* value, struct Environment* env);

#endif // !AST_H
