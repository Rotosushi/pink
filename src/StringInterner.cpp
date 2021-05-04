#include <stdlib.h>
#include <optional>
using std::optional;
#include <string>
#include <functional>
#include <algorithm>
#include <map>

#include "Utilities.hpp"
#include "StringInterner.hpp"


// inserts the name into the set of interned strings
// returns a pointer to the newly inserted string,
// if the name already exists, we simply return a pointer
// to the new result.
InternedString StringInterner::InternString(const std::string* str)
{
  // we maintain uniqueness by treating the
  // existing set of mappings/bindings as a
  // cache of values, we always query against
  // the cache and return the matching value
  // if it exists.
  // else we know we need to insert and return
  // the value we just inserted
  auto istr = std::find(interned_strings.begin(), interned_strings.end(), *str);

  if (istr != interned_strings.end())
  {
    // unwrap the iterator to a string, then
    // wrap that string in a bare pointer so
    // it can be cast into a InternedString
    return &(*istr);
  }
  else
  {
    interned_strings.emplace_back(*str);
    // return the address of the last element
    // (end() returning one-past-the-end as it were)
    return &(*(interned_strings.end()--));
  }


}

// searches for the interned string by using the pointer values.
// if we find it we return the pointer, otherwise
// this procedure simply returns null.
std::optional<InternedString> StringInterner::FindString(const string* str)
{
  auto istr = std::find(interned_strings.begin(), interned_strings.end(), *str);

  if (istr != interned_strings.end())
  {
    return std::optional<InternedString>(&(*istr));
  }
  else
  {
    return std::optional<InternedString>();
  }
}
