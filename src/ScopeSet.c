#include <stdint.h>
#include <stdbool.h>

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
}

bool ScopeSetIsFull(ScopeSet set)
{
  // if the last bit is set, we cannot
  // add any more sets to the representation
  // we have, so we must make this an error,
  // there are further complications if we want
  // to dynamically extend the size of our set
  // vector.
  if ((set & 0x80000000) > 0)
    return true;
  else
    return false;
}

// we use the ScopeSet, set, as the 'seed' value.
// we then use the outer scope to determine which
// bit needs to be flipped to add a new set, w.r.t.
// the 'outer' scope we are inserting this scope set into.
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
  result = set & (set | new);

  return result;
}


ScopeSet Subset(ScopeSet s0, ScopeSet s1)
{
  ScopeSet result = s0 & s1;
  return result;
}
