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
#include <vector>

namespace pink {

class ScopeSet
{
private:
  std::vector<bool> set; // TODO refactor this to use a vector of bools

  std::size_t MostSigBitIndex(); // this function becomes O(N) in the straightforward case.

  // we hid this constructor because whenever we are
  // introducing a new scope in the consumer code
  // we are doing it with respect to an existing scope.
  // even lambda's get built in the global scope. thus, their
  // scope designation is one past the global scope.
  // inner lambda's are the same discussion, one level deeper.
  // so all we have to implement is this 'one-past-ness' against
  // the knowns default of Global.
  ScopeSet(std::vector<bool>& set);
public:
  ScopeSet();
  ScopeSet(ScopeSet& set); // builds this scope relative to the given scope.

  ScopeSet IntroduceNewScope(ScopeSet& outer);

  ScopeSet Subset(ScopeSet& rhs);
};

}
