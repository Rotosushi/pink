#include <utility>

#include "aux/SymbolTable.h"

namespace pink {
    SymbolTable::SymbolTable()
        : map(), outer(nullptr)
    {

    }

    SymbolTable::SymbolTable(SymbolTable* o)
        : map(), outer(o)
    {

    }

    SymbolTable::~SymbolTable()
    {
        
    }

    SymbolTable* SymbolTable::OuterScope()
    {
        return outer;
    }

    llvm::Optional<llvm::Value*> SymbolTable::Lookup(InternedString symbol)
    {
        auto iter = map.find(symbol);
        if (iter == map.end())
            return llvm::Optional<llvm::Value*>();
        else
            return llvm::Optional<llvm::Value*>(iter->second);
    }

    void SymbolTable::Bind(InternedString symbol, llvm::Value* term)
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
            map.erase(iter);
        }
    }
}
