#pragma once
#include <utility> // std::pair<>

#include "llvm/ADT/APInt.h" // llvm::Optional<>
#include "llvm/ADT/DenseMap.h" // llvm::DenseMap<...>
#include "llvm/IR/Value.h" // llvm::Value

#include "aux/StringInterner.h" // pink::InternedString
#include "type/Type.h" // pink::Type


namespace pink {
/*
	SymbolTable maps names (symbols) to their type and value.
	we use our own internal representation of type here,
	because that is what we use to do the majority of typechecking,
	and we hold an llvm::Value because those are the only values 
	we generate during codegeneration. (to minimize the number of 
	translation steps.)
*/
    class SymbolTable {
    private:
        llvm::DenseMap<InternedString, std::pair<Type*, llvm::Value*>> map;
        SymbolTable* outer;

    public:
        SymbolTable();
        SymbolTable(const SymbolTable& other) = delete;
        SymbolTable(SymbolTable* outer);
        ~SymbolTable();

        SymbolTable* OuterScope();

        llvm::Optional<std::pair<Type*, llvm::Value*>> Lookup(InternedString symbol);
        llvm::Optional<std::pair<Type*, llvm::Value*>> LookupLocal(InternedString symbol);

        void Bind(InternedString symbol, Type* type, llvm::Value* term);
        void Unbind(InternedString symbol);
    };
}
