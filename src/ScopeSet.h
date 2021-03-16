#ifndef SCOPESET_H
#define SCOPESET_H
#include <stdbool.h>
#include <stdint.h>

/*
  a uint32_t is going to be our representation of
  a scope set. we are going to treat it like a vector
  of boolean values, where each bool is a flag that says,
  I belong to thus and such scope set. (we could call the
  first bit, set (a), the second, set (b), and so forth)

  this means testing for subset members is equivalent to a
  bitwise and of the two uint32_t's.
  testing for a full set of scopes is likewise a uint32_t
  comparison.

  adding a new scope to the scope set is a little more complex.
  it needs to be aware of the scope we are injecting into.
  (this is to be safe with macro expansion, which is treated
    as introducing a new scope within an outer scope.)

  if we observe the classical nesting of scopes, the solution
  seems very simple,
  a free variable occuring within a scope inherits the set of scopes
  it is bound within.
  a form which introduces a new scope does so first, before binding the
  associated identifier within that new local scope. this means that
  free occurances of any variable within the local scope are now
  associated with the global scope and the new scope.

  observing our bit vector as it represents each of these states

  global ScopeSet [0000....0001]

  lambda ScopeSet [0000....0011]
*/

typedef uint32_t ScopeSet;


bool ScopeSetIsFull(ScopeSet set);

// we use the ScopeSet, set, as the 'seed' value.
// we then use the outer scope to determine which
// bit needs to be flipped to add a new set, w.r.t.
// the 'outer' scope we are inserting this scope set into.
// then the 'seed' value has it's contents,
// plus the newly flipped bit joined in the result value.

// returns a ScopeSet which is equivalent to Set, except it
// has a new scope added, with respect to the active 'outer'
// scope.
ScopeSet IntroduceNewScope(ScopeSet set, ScopeSet outer);

// returns the subset of the two passed in ScopeSets.
ScopeSet Subset(ScopeSet s0, ScopeSet s1);

#endif // !SCOPESET_H
