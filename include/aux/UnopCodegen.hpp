#pragma once

#include "llvm/IR/Value.h"

#include "aux/Error.hpp"
#include "aux/Outcome.hpp"
#include "aux/Environment.hpp"

#include "type/Type.hpp"

namespace pink {
    typedef Outcome<Error, llvm::Value*> (*UnopCodegenFn)(llvm::Value* term, Environment& env);

    class UnopCodegen {
    public:
        Type*         result_type;
        UnopCodegenFn generate;

        UnopCodegen() = delete;
        UnopCodegen(const UnopCodegen& other);
        UnopCodegen(Type* rt, UnopCodegenFn gen);
    };
}
