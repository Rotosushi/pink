#include "kernel/Gensym.h"

namespace pink {
// this is a somewhat naieve implementation.
// a stronger garuntee would be to check that
// the symbol generated is unique within the
// local environment. as it stands now it is simply
// unlikely that the symbol would be taken.
auto Gensym(const std::string &seed) -> std::string {
  static size_t index = 0;
  std::string result = "__" + seed;
  result += std::to_string(index);
  index++;
  return result;
}
} // namespace pink