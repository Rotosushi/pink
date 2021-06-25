#include <cstdint>
#include <utility>
#include <vector>
#include <optional>
#include <algorithm>

#include "Associativity.hpp"
#include "StringInterner.hpp"

namespace pink {

BinopPredenceTable::BinopPredenceTable()
{

}

/*
  searches the table for an existing operator,
  should one exist we return that (Prec, Assoc)?,
  if no operator has been bound yet, we insert
  the new operator into the end.

*/
std::optional<std::pair<Associativity, Precedence>> BinopPrecedenceTable::RegisterBinopPrecedenceAssociativity(InternedString operator, Associativity associativity, Precedence precedence)
{
  auto operators_match = [operator](const std::pair<InternedString, std::pair<Associativity, Precedence>>& element)
  {
    if (element.first() == operator)
      return true;
    else
      return false;
  };

  std::vector<std::pair<InternedString, std::pair<Associativity, Precedence>>>::iterator itr = std::find_if(table.begin(), table.end(), operators_match);

  if (itr == table.end())
  {
    std::pair<Associativity, Precedence> elem = (table.emplace(itr, operator, std::make_pair(associativity, precedence)))->second();
    return std::make_optional(elem);
  }
  else
  {
    return std::optional<std::pair<Associativity, Precedence>>(itr->second());
  }
}

std::optional<std::pair<Associativity, Precedence>> BinopPrecedenceTable::Lookup(InternedString operator)
{
  auto operators_match = [operator](const std::pair<InternedString, std::pair<Associativity, Precedence>>& element)
  {
    if (element.first() == operator)
      return true;
    else
      return false;
  };

  std::vector<std::pair<InternedString, std::pair<Associativity, Precedence>>>::iterator itr = std::find_if(table.begin(), table.end(), operators_match);

  if (itr == table.end())
  {
    return std::optional<std::pair<Associativity, Precedence>>();
  }
  else
  {
    return std::optional<std::pair<Associativity, Precedence>>(itr->second());
  }
}

}
