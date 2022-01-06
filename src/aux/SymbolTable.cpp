#include <utility>

#include "aux/SymbolTable.hpp"

namespace pink {
    SymbolTable::SymbolTable()
        : map(), outer(nullptr)
    {

    }

    SymbolTable::SymbolTable(const SymbolTable& other)
        : outer(other.outer)
    {
        for (auto pair : other.map)
        {
            map.insert(std::make_pair(pair.first, pair.second->Clone()));
        }
    }

    SymbolTable::SymbolTable(SymbolTable* o)
        : map(), outer(o)
    {

    }

    SymbolTable::~SymbolTable()
    {
        // The SymbolTable assumes ownership of the bound terms.
        for (auto pair : map)
        {
            Ast* term = pair.second;

            if (term != nullptr)
                delete term;
        }
    }

    SymbolTable* SymbolTable::OuterScope()
    {
        return outer;
    }

    llvm::Optional<Ast*> SymbolTable::Lookup(InternedString symbol)
    {
        auto iter = map.find(symbol);
        if (iter == map.end())
            return llvm::Optional<Ast*>();
        else
            return llvm::Optional<Ast*>(iter->second);
    }

    void SymbolTable::Bind(InternedString symbol, Ast* term)
    {
        map.insert(std::make_pair(symbol, term));
    }

    void SymbolTable::Unbind(InternedString symbol)
    {
        auto iter = map.find(symbol);

        if (iter == map.end())
            return;
        else
        {
            // The SymbolTable assumes ownership of the bound terms.
            // so we must free that memory before we can remove the
            // entry from the DenseMap
            Ast* term = iter->second;

            if (term != nullptr)
                delete term;

            map.erase(iter);
        }
    }
}
