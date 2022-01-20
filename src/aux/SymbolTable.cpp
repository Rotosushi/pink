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

    llvm::Optional<std::pair<Type*, llvm::Value*>> SymbolTable::Lookup(InternedString symbol)
    {
        auto iter = map.find(symbol);
        if (iter == map.end())
            return llvm::Optional<std::pair<Type*, llvm::Value*>>();
        else
            return llvm::Optional<std::pair<Type*, llvm::Value*>>(iter->second);
    }

    void SymbolTable::Bind(InternedString symbol, Type* type, llvm::Value* term)
    {
        map.insert(std::make_pair(symbol, std::make_pair(type, term)));
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
