#include <stdint.h>
#include <stdbool.h>

#include "Error.h"
#include "ScopeSet.h"

// https://stackoverflow.com/questions/2589096/find-most-significant-bit-left-most-that-is-set-in-a-bit-array
uint32_t MostSigBitIndex(uint32_t v)
{
  static const int DeBruijnTable[32] =
  {
    0, 9, 1, 10,
    13, 21, 2, 29,
    11, 14, 16, 18,
    22, 25, 3, 30,
    8, 12, 20, 28,
    15, 17, 24, 7,
    19, 27, 23, 6,
    26, 5, 4, 31
  };

  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;

  uint32_t result = DeBruijnTable[((uint32_t)(v * 0x07C4ACDDU)) >> 27];
  return result;
}

bool ScopeSetIsFull(ScopeSet set)
{
  // if the last bit is set, we cannot
  // add any more sets to the representation
  // we have, so we must make this an error,
  // there are further complications if we want
  // to dynamically extend the size of our set
  // vector. (we need to loop this action over
  // each 32 bit word that occurs in the dynamic
  // set, and collate the results such that we only
  // return the highest found. this also further introduces
  // the complication that we may take a small scope
  // and inject it into a much larger scope, which would
  // need to naturally promote the size of the smaller scope
  // to the size of the larger scope if we wanted to
  // push a new active scope, and this further introduces
  // the case where we have to grow both sets because the
  // scope we want to add is one past the end of the larger
  // bigger container.)
  // not to mention the performance considerations of associating
  // every single variable and scoped term with a bit vector
  // whose size is dependant upon the depth of the scope being
  // expressed. (instead of a constant size overhead per term)
  // though if we are talking about being able to
  // compute along side introducing macros, we probably need more
  // that 31 levels of scopes. though, i haven't ever worked with
  // macros, so i don't have any idea how much depth is regularly
  // expressed. however, if we imagine using recursion to do some
  // work within macros, we can be talking about introducing a potentially
  // arbitrary number of scopes.

  if ((set & 0x80000000) > 0)
    return true;
  else
    return false;
}

// we use the ScopeSet, set, as the 'seed' value.
// we then use the outer scope to determine which
// bit needs to be flipped to add a new set, w.r.t.
// the 'outer' scope we are inserting this scope set into.
// the normal scope sequence introduces scopes such that
// both of these values are the same scope, i.e. introducing
// a new procedure definition within the body of another
// procedure, the outer scope is the outer procedures scope,
// the new scope we are constructing is the scope we wish to
// assign to the new lambda, so, the new lambda's scope is
// identical to the outer scope plus the single additional bit.
// and since there is no way of introducing new names into the
// old scope, we cannot have a situation as described in the
// paper I got this idea from. lexical macros introduce older
// bindings into newer scopes, this means we need to be careful
// to add the single new bit to the result, but we want to avoid
// bringing any scope into the new one that was not definined in the
// macro itself, or is the new bit being introduced. any level of
// scope nesting that has occured from the definition of the term
// to the application of the macro needs to be left zeroed out in
// the scope of the names introduces by the macro. this allows the
// macro to freely inject names into the scope without worrying
// about conflicts, because the names it introduces are scopes such
// that they will not bind to any matching name that was introduced
// within the nested scopes, and will instead bind to the name that
// was defined to be used with the macro.
// note how this is distinct from a closure, which brings values into
// the new definition, vs, macros which bring terms. the clousre action
// is a runtime one, and the macro action a compile time one.
// then the 'seed' value has it's contents,
// plus the newly flipped bit joined in the result value.
ScopeSet IntroduceNewScope(ScopeSet set, ScopeSet outer)
{
  ScopeSet result = 0, new = 0;

  if (ScopeSetIsFull(outer))
  {
    FatalError("Cannot represent more than 31 levels of nested scopes, aborting!", __FILE__, __LINE__);
  }
  if (outer == 0)
  {
    new = 1;
  }
  else
  {
    // the new scope is represented as the next
    // most significant bit being set within the
    // ScopeSet.
    new = 1 << (1 + MostSigBitIndex(outer));
  }
  // set all bits in result, that are set in both the
  // set and the new scope sets.
  result = set | new;

  return result;
}


ScopeSet Subset(ScopeSet s0, ScopeSet s1)
{
  ScopeSet result = s0 & s1;
  return result;
}
