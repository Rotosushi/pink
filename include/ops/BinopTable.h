#pragma once
#include <utility> // std::pair<>
#include <memory>  // std::unique_ptr

#include "llvm/ADT/APInt.h" // llvm::Optional<>
#include "llvm/ADT/DenseMap.h" // llvm::DenseMap<>

#include "aux/StringInterner.h"

#include "ops/BinopLiteral.h"


namespace pink {
    class BinopTable {
    private:
        llvm::DenseMap<InternedString, std::unique_ptr<BinopLiteral>> table;

    public:
        BinopTable();
        ~BinopTable();

        std::pair<InternedString, BinopLiteral*> Register(InternedString op, Precedence p, Associativity a);
        std::pair<InternedString, BinopLiteral*> Register(InternedString op, Precedence p, Associativity a, Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn);

        void Unregister(InternedString op);

        llvm::Optional<std::pair<InternedString, BinopLiteral*>> Lookup(InternedString op);
    };
}
