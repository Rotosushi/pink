#pragma once
#include <vector>
#include <cstdint> // int8_t
#include <utility> // std::pair

#include "llvm/ADT/APInt.h" // llvm::Optional

#include "aux/StringInterner.hpp" // pink::InternedString

namespace pink {
    enum class Associativity {
        None,
        Left,
        Right,
    };

    typedef int8_t Precedence;

    class PrecedenceTable {
    private:
        std::vector<std::pair<InternedString, std::pair<Associativity, Precedence>>> table;

    public:
        PrecedenceTable();
        ~PrecedenceTable();

        void Register(InternedString op, Associativity associativity, Precedence precedence);
        llvm::Optional<std::pair<Associativity, Precedence>> Lookup(InternedString op);
    };
}
