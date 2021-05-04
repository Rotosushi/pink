#include <cstdint>

#include "PinkError.hpp"
#include "ScopeSet.hpp"

ScopeSet::ScopeSet()
{
  this->set = 1;
}

ScopeSet::ScopeSet(uint32_t set)
{
  this->set = set;
}

// https://stackoverflow.com/questions/2589096/find-most-significant-bit-left-most-that-is-set-in-a-bit-array
uint32_t ScopeSet::MostSigBitIndex()
{
  // this procedure is the reason we use uint32_t for
  // out ScopeSet representation. this math only works
  // on bit vectors of size 32. were we to support
  // arbitrary depth of scopes, we would need to repeat
  // this operation on each 32 bit splice of the total
  // depth, and return only the largest present value.
  // we could additionally imagine optimizations such as
  // starting at the highest position in the larger of the
  // two vectors and working backwards. which allows an early return.
  // this has implications on comparing two ScopeSet
  // objects, which now need to contend with the fact that
  // any two given ScopeSets ~may~ have different sizes.
  // there are consequences to adopting this method of
  // scoping,
  // for instance, we cannot represent integers encoded as
  // functions (church numbers) greater than the integer value 31.
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

  set |= set >> 1;
  set |= set >> 2;
  set |= set >> 4;
  set |= set >> 8;
  set |= set >> 16;

  uint32_t result = DeBruijnTable[((uint32_t)(set * 0x07C4ACDDU)) >> 27];
  return result;
}

bool ScopeSet::operator <(ScopeSet rhs)
{
  return (this->set < rhs.set);
}

bool ScopeSet::operator <=(ScopeSet rhs)
{
  return (this->set <= rhs.set);
}

bool ScopeSet::IsFull()
{
  if ((set & 0x80000000) > 0)
    return true;
  else
    return false;
}

// builds a new scopeset with a new scope appended
// one bit position after the referenced outer scope,
// but with the rest of the values identical to
// this ScopeSet.
// i.e. this->set --> [0000 0000 ... 0000 0000]
//     outer->set --> [0000 0000 ... 0001 1111]
//    result->set --> [0000 0000 ... 0011 1111]
//
//      this->set --> [0000 0000 ... 0000 0111]
//     outer->set --> [0000 0000 ... 0011 1111]
//    result->set --> [0000 0000 ... 0100 0111]
// according to the paper i read describing scope sets,
// this behavior is what gives Racket clean macros.
// because lookup depends on this other dataset, alongside
// names, and which is maintained in lockstep with the
// scopes we construct.
ScopeSet ScopeSet::IntroduceNewScope(ScopeSet outer)
{
  uint32_t result = 0, tmp = 0;

  if (outer.IsFull())
  {
    FatalError("Cannot represent more than 31 levels of scope", __FILE__, __LINE__);
  }

  uint32_t bitIndex = outer.MostSigBitIndex();

  if (bitIndex == 0)
  {
    tmp = 1;
  }
  else
  {
    tmp = 1 << (1 + bitIndex);
  }

  result = set | tmp;
  return ScopeSet(result);
}

// returns the subset of this scopeset and the given outer scopeset.
ScopeSet ScopeSet::Subset(ScopeSet outer)
{
  ScopeSet result(set & outer.set);
  return result;
}
