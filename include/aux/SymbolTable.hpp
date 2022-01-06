#pragma once

#include "llvm/ADT/APInt.h" // llvm::Optional
#include "llvm/ADT/DenseMap.h" // llvm::DenseMap<...>

#include "aux/StringInterner.hpp"
#include "ast/Ast.hpp"


namespace pink {
    class SymbolTable {
    private:
        /*
            Why use a bare pointer here?
            because it will save space on
            every term inserted into the
            table. at the cost of time during
            destruction, when each symbol binding
            needs deallocated.
        */
        llvm::DenseMap<InternedString, Ast*> map;
        SymbolTable* outer;

    public:
        SymbolTable();
        SymbolTable(const SymbolTable& other);
        SymbolTable(SymbolTable* outer);
        ~SymbolTable();

        SymbolTable* OuterScope();

        llvm::Optional<Ast*> Lookup(InternedString symbol);

        void Bind(InternedString symbol, Ast* value);
        void Unbind(InternedString symbol);
    };
}
