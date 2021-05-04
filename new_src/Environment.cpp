

#include "Environment.hpp"


Environment::Environment(SymbolTable* symbs, StringInterner* Inames)
{
  interned_names = Inames;
  symbols        = symbs;
}
