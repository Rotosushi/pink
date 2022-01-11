#pragma once
#include "llvm/IR/Value.h"

#include "aux/Error.hpp"
#include "aux/Outcome.hpp"
#include "aux/Environment.hpp"

#include "type/Type.hpp"


namespace pink {
    typedef Outcome<Error, llvm::Value*> (*BinopCodegenFn)(llvm::Value* left, llvm::Value* right, Environment& env);

    class BinopCodegen {
    public:
        Type* result_type;
        BinopCodegenFn generate;

        BinopCodegen() = delete;
        BinopCodegen(const BinopCodegen& other);
        BinopCodegen(Type* ret_t, BinopCodegenFn fn);
    };
}
