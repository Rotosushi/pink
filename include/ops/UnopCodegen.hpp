#pragma once


#include "llvm/IR/Value.h"

#include "aux/Error.hpp"
#include "aux/Outcome.hpp"

#include "type/Type.hpp"

namespace pink {
    class Environment;

    typedef Outcome<llvm::Value*, Error> (*UnopCodegenFn)(llvm::Value* term, Environment& env);

    class UnopCodegen {
    public:
        Type*         result_type;
        UnopCodegenFn generate;

        UnopCodegen() = delete;
        UnopCodegen(const UnopCodegen& other);
        UnopCodegen(Type* rt, UnopCodegenFn gen);
    };
}
