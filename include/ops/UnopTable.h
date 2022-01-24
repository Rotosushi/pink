#pragma once
#include <utility> // std::pair
#include <memory>  // std::unique_ptr

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMap.h"

#include "aux/StringInterner.h"

#include "ops/UnopLiteral.h"

namespace pink {
    class UnopTable {
    private:
        // https://llvm.org/doxygen/DenseMap_8h.html
        llvm::DenseMap<InternedString, std::unique_ptr<UnopLiteral>> table;

    public:
        UnopTable();
        ~UnopTable();

        std::pair<InternedString, UnopLiteral*> Register(InternedString op);
        std::pair<InternedString, UnopLiteral*> Register(InternedString op, Type* arg_t, Type* ret_t, UnopCodegenFn fn);

        void Unregister(InternedString op);

        llvm::Optional<std::pair<InternedString, UnopLiteral*>> Lookup(InternedString op);
    };
}
