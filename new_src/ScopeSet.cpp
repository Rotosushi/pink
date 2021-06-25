#include <vector>

#include "Error.hpp"
#include "ScopeSet.hpp"

namespace pink {

ScopeSet::ScopeSet()
  : set()
{
  set.push_back(true);
  set.push_back(false);
}

ScopeSet(std::vector<bool>& set)
  : set(set)
{

}

ScopeSet::ScopeSet(ScopeSet& set)
  : set ((set.IntroduceNewScope(set)).set)
{

}

std::size_t ScopeSet::MostSigBitIndex()
{
  // walk from the end and return the index of the first true value.
  // this -has- to be the rightmost set bit in our vector, that is
  // our latest active scope in this set.
  for (std::size_t index = set.size(); index > 0; index--)
  {
    if (set[index] == true)
      return index;
  }
}

// builds a new scopeset with a new scope appended
// one bit position after the referenced outer scope,
// but with the rest of the values identical to
// this ScopeSet.
// i.e. this->set --> [0000 0000 ... 0000 0001]
//     outer->set --> [0000 0000 ... 0001 1111]
//    result->set --> [0000 0000 ... 0010 0001]
//
//      this->set --> [0000 0000 ... 0000 0111]
//     outer->set --> [0000 0000 ... 0011 1111]
//    result->set --> [0000 0000 ... 0100 0111]
// according to the paper i read describing scope sets,
// this behavior is what gives Racket clean macros.
// because lookup depends on this other dataset, alongside
// names, and which is maintained in lockstep with the
// scopes we construct.
ScopeSet ScopeSet::IntroduceNewScope(ScopeSet& outer)
{
  std::size_t setsz = set.size(), outersz = outer.set.size();
  std::size_t ressz = setsz > outersz ? setsz + 1 : outersz + 1;

  std::vector<bool> result_set;
  result_set.reserve(ressz);

  /*
    walk three arrays with one index,
    the result array at the index is only
    set to true if the cells of both other
    arrays at that index are true.
    if we walk past the end of one array, but
    we can still process the other array,
    we imagine that the other array simply
    contains zeroes/falses in all cells past that
    point. and then we simplify all of those
    ands against zero by setting the result array
    to false in this case.
  */
  for (std::size_t idx = 0; idx < ressz; idx++)
  {
    if ((idx < setsz) && (idx < outersz))
      result_set[idx] = set[idx] && outer.set[idx];
    else
      result_set[idx] = false;
  }

  // add a new scope one past the end of the outer scope.
  std::size_t pos = outer.MostSigBitIndex();
  result_set[pos + 1] = true;
  return ScopeSet(result_set);
}

// returns the subset of this scopeset and the given outer scopeset.
ScopeSet ScopeSet::Subset(ScopeSet outer)
{
  std::size_t setsz = set.size(), outersz = outer.set.size();
  std::size_t ressz = setsz > outersz ? setsz + 1 : outersz + 1;

  std::vector<bool> result_set;
  result_set.reserve(ressz);

  for (std::size_t idx = 0; idx < ressz; idx++)
  {
    if ((idx < setsz) && (idx < outersz))
      result_set[idx] = set[idx] && outer.set[idx];
    else
      result_set[idx] = false;
  }

  return ScopeSet(result_set);
}

}
