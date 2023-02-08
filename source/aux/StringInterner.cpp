#include "aux/StringInterner.h"

namespace pink {

auto StringInterner::Intern(std::string_view str) -> InternedString {
  auto elem = set.try_emplace(str); // the table allocates a copy of the view
  InternedString result = (elem.first->getKey()).data();
  return result;
}

} // namespace pink
