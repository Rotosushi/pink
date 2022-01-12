#pragma once
#include <utility> // std::pair<>

#include "llvm/ADT/APInt.h" // llvm::Optional<>
#include "llvm/ADT/DenseMap.h" // llvm::DenseMap<>

#include "aux/BinopLiteral.hpp"
#include "aux/StringInterner.hpp"

namespace pink {
    class BinopTable {
    private:
        llvm::DenseMap<InternedString, BinopLiteral*> table;

    public:
        BinopTable();
        ~BinopTable();

        std::pair<InternedString, BinopLiteral*> Register(InternedString op, Precedence p, Associativity a);
        std::pair<InternedString, BinopLiteral*> Register(InternedString op, Precedence p, Associativity a, Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn);

        void Unregister(InternedString op);

        llvm::Optional<std::pair<InternedString, BinopLiteral*>> Lookup(InternedString op);
    };
}
