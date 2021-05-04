#pragma once
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
#include <cstdint>


class ScopeSet
{
private:
  uint32_t set;

  uint32_t MostSigBitIndex();
public:
  ScopeSet();
  ScopeSet(uint32_t set);

  bool operator<(ScopeSet rhs);
  bool operator<=(ScopeSet rhs);

  bool IsFull();

  ScopeSet IntroduceNewScope(ScopeSet outer);

  ScopeSet Subset(ScopeSet rhs);
};
