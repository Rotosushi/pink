/**
 * @file StringInterner.h
 * @brief Header for class StringInterner
 * @version 0.1
 *
 */
#pragma once
#include <string>

#include <llvm/ADT/StringSet.h>

namespace pink {
using InternedString = char const *;

class StringInterner {
private:
  llvm::StringSet<> set;

public:
  inline auto Intern(std::string_view str) -> InternedString {
    // #NOTE: llvm::StringSet allocates a copy of the view
    auto set_element = set.try_emplace(str);
    auto view        = set_element.first->getKey();
    return view.data();
  }
};

} // namespace pink
