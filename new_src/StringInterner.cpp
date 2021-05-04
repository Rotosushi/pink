#include <string>
#include <utility>

#include "llvm/ADT/StringSet.h"

#include "StringInterner.hpp"

InternedString StringInterner::InternString(const std::string& str)
{
  // try_emplace encompases both actions of InternString
  // if the string already exists, the set is not updated
  // and the existing element is returned, if the element
  // does not exist, the set is updated with the new element.
  // and that new element is what is retured.
  // in order to make using this proceure easy, all of the
  // LLVM types are scrubbed from this classes interface, and we
  // return a bare char* as the result. this allows consuming
  // code to treat the InternedString as a string, or as a
  // integer value which represents this unique string.
  // and thus, client code need only compare pointer values
  // to compare the two strings. (internally strcmp needs to
  // be used, but all that and the memory is handled by the
  // StringSet itself.)
  auto element = strs.try_emplace(str);
  return (InternedString)((element.first)->getKey()).data();
}
