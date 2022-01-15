#pragma once

#include "llvm/ADT/APInt.h" // llvm::Optional
#include "llvm/ADT/DenseMap.h" // llvm::DenseMap<...>
#include "llvm/IR/Value.h" // llvm::Value

#include "aux/StringInterner.h"
#include "ast/Ast.h"


namespace pink {
    class SymbolTable {
    private:
        llvm::DenseMap<InternedString, llvm::Value*> map;
        SymbolTable* outer;

    public:
        SymbolTable();
        SymbolTable(const SymbolTable& other) = delete;
        SymbolTable(SymbolTable* outer);
        ~SymbolTable();

        SymbolTable* OuterScope();

        llvm::Optional<llvm::Value*> Lookup(InternedString symbol);

        void Bind(InternedString symbol, llvm::Value* value);
        void Unbind(InternedString symbol);
    };
}
