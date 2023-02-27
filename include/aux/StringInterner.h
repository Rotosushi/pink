/**
 * @file StringInterner.h
 * @brief Header for class StringInterner
 * @version 0.1
 *
 */
#pragma once
#include "llvm/ADT/StringSet.h"

namespace pink {
using InternedString = char const *;

class StringInterner {
private:
  llvm::StringSet<> set;

public:
  inline auto Intern(std::string_view str) -> InternedString {
    return set.try_emplace(str).first->getKey().data();
  }
};

} // namespace pink
