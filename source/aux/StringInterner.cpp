#include "aux/StringInterner.h"

namespace pink {

auto StringInterner::Intern(const char *str) -> InternedString {
  auto elem = set.try_emplace(str);
  InternedString result = (elem.first->getKey()).data();
  return result;
}

auto StringInterner::Intern(std::string &str) -> InternedString {
  auto elem = set.try_emplace(str);
  InternedString result = (elem.first->getKey()).data();
  return result;
}

} // namespace pink
