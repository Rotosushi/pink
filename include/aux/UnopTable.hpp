#pragma once
#include <utility> // std::pair

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMap.h"

#include "aux/StringInterner.hpp"
#include "aux/UnopLiteral.hpp"

namespace pink {
    class UnopTable {
    private:
        // https://llvm.org/doxygen/DenseMap_8h.html
        llvm::DenseMap<InternedString, UnopLiteral*> table;

    public:
        UnopTable();
        ~UnopTable();

        std::pair<InternedString, UnopLiteral*> Register(InternedString op);
        std::pair<InternedString, UnopLiteral*> Register(InternedString op, Type* arg_t, Type* ret_t, UnopCodegenFn fn);

        void Unregister(InternedString op);

        llvm::Optional<std::pair<InternedString, UnopLiteral*>> Lookup(InternedString op);
    };
}
