#pragma once
#include <iostream>
#include <string_view>

namespace pink {
[[noreturn]] inline void
FatalError(const char *dsc, const char *file, size_t line) {
  std::cerr << "Fatal Error in file: [" << file << "] line: [" << line << "]"
            << " description: [" << dsc << "]\n";
  exit(1);
}

[[noreturn]] inline void
FatalError(std::string_view dsc, const char *file, size_t line) {
  FatalError(dsc.begin(), file, line);
}

} // namespace pink