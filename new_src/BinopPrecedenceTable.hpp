#pragma once
#include <cstdint>
#include <utility>
#include <vector>
#include <optional>

#include "Associativity.hpp"
#include "StringInterner.hpp"


namespace pink {

typedef uint32_t Precedence;

class BinopPrecedenceTable
{
private:
  std::vector<std::pair<InternedString, std::pair<Associativity, Precedence>>> table;

public:
  BinopPredenceTable();

  std::optional<std::pair<Associativity, Precedence>> RegisterBinopPrecedenceAssociativity(InternedString operator, Associativity associativity, Precedence precedence);
  std::optional<std::pair<Associativity, Precedence>> Lookup(InternedString operator);
};

}
