
#include "aux/PrecedenceTable.hpp"


namespace pink {
    PrecedenceTable::PrecedenceTable()
        : table()
    {

    }

    PrecedenceTable::~PrecedenceTable()
    {

    }

    void PrecedenceTable::Register(InternedString op, Associativity associativity, Precedence precedence)
    {
        bool found = false;
        for (auto elem : table)
        {
            if (std::get<InternedString>(elem) == op)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            table.emplace_back(op, std::make_pair(associativity, precedence));
        }
    }

    llvm::Optional<std::pair<Associativity, Precedence>> PrecedenceTable::Lookup(InternedString op)
    {
        for (auto elem : table)
        {
            if (std::get<InternedString>(elem) == op)
            {
                return llvm::Optional<std::pair<Associativity, Precedence>>(std::get<1>(elem));
            }
        }

        return llvm::Optional<std::pair<Associativity, Precedence>>();
    }
}
