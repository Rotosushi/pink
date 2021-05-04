#ifndef PROCTYPE_H
#define PROCTYPE_H

struct Type;

// so, this makes simple sense to have these pointers
// be Type*'s, however, this clutters up the code when
// dealing with parsing types, (for subtle parsing reasons)
// and forces us into useing at least two Judgement types.
// one for types and one for terms.
// It does work out fine for annotations, Lambda's just held a
// Type* and so would Bind's.
//  so, to simplify construction we build
// a type out of two things: the thing representing the abstraction
// of a type. (Type) and the thing which represents an instance
// of said type within the living program, TypeLiteral. (something which
// has a location essentially.) in the earliest versions of Pink
// Type itself held the location data, and when anything had a Type,
// it got a new instance of the same type. hence if two names had
// integer type, they would literally return two separate allocations
// of a thing representing integer types. now, this was inneficient
// because comparing two types now means comparing via induction.
// which requires physically walking two type trees and comparing
// each. it is also inefficient in that every type object which
// represents the same type idea is redundant with instances of itself.
// this is the basic idea behind interning types, we replace every instance
// of an integer Type within the program with one singleton instance.
// this means that every object within the program that 'has' integer type
// has a pointer to the same integer type. this means that the pointer
// value itself is synonymous with the Integer Type value.
// so to compare two types as equal means comparing their pointer values.
// so these are Ast*'s because the subroutine which parses
// term '+' term, is the same code as which parses term '->' term
// with a different symbol between the two.
// so, we can write a smaller parser if we can wrap a (5)
// in the same type that we wrap a (Int).
// of course, this implies that the parser knows something about
// the Type of the things that it is parsing. (emmiting an error
// when we parse (5) -> Int which is malformed.)
// so to make types compose together in the same way that binops
// compose together, we must store types within Ast* structures.
// this does enable us to call the same Equals routine to compare
// Types and Values.
// turns out, this simply isn't true, and i figured out how to
// do both. ProcType is built out of Type*'s directly, and TypeLiterals
// only ever hold a single Type*. there is no case where an instance of
// a type ever holds anything other than a Type*. 
typedef struct ProcType
{
  struct Type* lhs;
  struct Type* rhs;
} ProcType;

void InitializeProcType(ProcType* proc, struct Type* left, struct Type* right);

void DestroyProcType(ProcType* proc);

void CloneProcType(ProcType* destination, ProcType* source);

char* ToStringProcType(ProcType* proc);

bool EqualProcTypes(ProcType* t1, ProcType* t2);
#endif // !PROCTYPE_H
