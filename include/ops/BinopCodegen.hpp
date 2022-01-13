#pragma once
#include "llvm/IR/Value.h"

#include "aux/Error.hpp"
#include "aux/Outcome.hpp"

#include "type/Type.hpp"


namespace pink {
    // This forward declaration is needed to break the circular dependancy
    // that would be created by the fact that pink::Environment requires
    // the definition of pink::BinopTable, and pink::BinopTable needs
    // the definition of pink::BinopLiteral, and pinkBinopLiteral needs
    // the definition of pink::BinopCodegen, which needs the definition of
    // pink::Environment.
    class Environment;

    typedef Outcome<llvm::Value*, Error> (*BinopCodegenFn)(llvm::Value* left, llvm::Value* right, Environment& env);

    class BinopCodegen {
    public:
        Type* result_type;
        BinopCodegenFn generate;

        BinopCodegen() = delete;
        BinopCodegen(const BinopCodegen& other);
        BinopCodegen(Type* ret_t, BinopCodegenFn fn);
    };
}
