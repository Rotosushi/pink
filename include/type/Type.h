#pragma once
#include <variant>

#include "type/All.h"

namespace pink {
// so how do we represent interned trees
// of types with a type that is a variant?
// the safest solution would be to have
// the type interner hand out shared pointers.
// and for the types themselves to hold shared
// pointers. But that is way more overhead
// compared to the current solution.
// actually, the type interner could still work,
// each type interning location must be a variant
// however, we cannot intern the derived types directly.
// (which adds some overhead)
// what is breaking is that the type's that are returned
// out of the "Get*Type" functions must be Type*
// (pointers to the variant.) and as such there
// would be no way of conviently querying the
// state of the derived type in functions that
// needed that information. Typecheck, and the Type
// visitors.
/*using Type = std::variant<ArrayType,
                          BooleanType,
                          CharacterType,
                          FunctionType,
                          TypeVariable,
                          IntegerType,
                          NilType,
                          PointerType,
                          SliceType,
                          TupleType,
                          VoidType>;
*/
// this is however closer to my idea
// of compacting the tree's into a deque
// like type. ideally each basic block becomes
// a single vector allocation, where only the
// other basic block terms point out of the
// allocation, otherwise if a parent term has
// children terms, they are allocated adjacent in
// the vector. Unfortunately that idea is probably
// far too convoluted to be much of a performance win,
// especially given how much simpler a tree is
// implementation wise. additionally there are concerns
// in that you cannot rearrange the tree at all in this
// configuration, in fact you have to know exactly how
// large a basic block will be before you allocate it.
// which adds another pass over the AST itself. Thus
// there will need to be two abstract syntax tree types
// maintained in lock-step, which is a lot of work.
} // namespace pink
