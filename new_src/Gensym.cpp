#include "Gensym.hpp"

namespace pink {

std::string Gensym(std::string prefix)
{
  static std::size_t symcnt = 0;
  std::string result = prefix + std::to_string(symcnt++);
  return result;
}

}
