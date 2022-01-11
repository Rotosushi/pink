#pragma once
#include <utility> // std::pair

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMap.h"

#include "aux/UnopCodegen.hpp"

namespace pink {
    class UnopLiteral {
    private:
        // https://llvm.org/doxygen/DenseMap_8h.html
        llvm::DenseMap<Type*, UnopCodegen*> overloads;

    public:
        UnopLiteral();
        UnopLiteral(Type* arg_t, Type* ret_t, UnopCodegenFn fn);
        ~UnopLiteral();

        std::pair<Type*, UnopCodegen*> Register(Type* arg_t, Type* ret_t, UnopCodegenFn fn);
        void Unregister(Type* a);

        llvm::Optional<std::pair<Type*, UnopCodegen*>> Lookup(Type* arg_t);
    };
}
