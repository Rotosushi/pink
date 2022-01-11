#pragma once
#include <utility> // std::pair

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMap.h"

#include "aux/BinopCodegen.hpp"
#include "aux/PrecedenceAndAssociativity.hpp"

namespace pink {

    class BinopLiteral {
    private:
        // because pink::Types are interned, std::pair<>::operator ==
        // works how we would want it too.
        llvm::DenseMap<std::pair<Type*, Type*>, BinopCodegen*> overloads;

    public:
        Precedence precedence;
        Associativity associativity;

        BinopLiteral() = delete;
        BinopLiteral(Precedence p, Associativity a);
        BinopLiteral(Precedence p, Associativity a, Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn);

        std::pair<std::pair<Type*, Type*>, BinopCodegen*> Register(Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn);

        void Unregister(Type*  left_t, Type* right_t);

        llvm::Optional<std::pair<std::pair<Type*, Type*>, BinopCodegen*>> Lookup(Type* left_t, Type* right_t);
    };
}
