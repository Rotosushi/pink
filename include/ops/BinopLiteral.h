#pragma once
#include <utility> // std::pair
#include <memory>  // std::unique_ptr

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMap.h"

#include "ops/BinopCodegen.h"
#include "ops/PrecedenceAndAssociativity.h"

namespace pink {

    class BinopLiteral {
    private:
        // because pink::Types are interned, std::pair<>::operator ==
        // works how we would want it too.
        llvm::DenseMap<std::pair<Type*, Type*>, std::unique_ptr<BinopCodegen>> overloads;

    public:
        Precedence precedence;
        Associativity associativity;

        BinopLiteral() = delete;
        BinopLiteral(Precedence p, Associativity a);
        BinopLiteral(Precedence p, Associativity a, Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn);
        ~BinopLiteral();

        std::pair<std::pair<Type*, Type*>, BinopCodegen*> Register(Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn);

        void Unregister(Type*  left_t, Type* right_t);

        llvm::Optional<std::pair<std::pair<Type*, Type*>, BinopCodegen*>> Lookup(Type* left_t, Type* right_t);
    };
}
