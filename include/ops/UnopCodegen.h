#pragma once


#include "llvm/IR/Value.h"

#include "aux/Error.h"
#include "aux/Outcome.h"

#include "type/Type.h"

namespace pink {
    class Environment;

	// #TODO: I suppose that technically, we should 
	// catch any errors before the point of calling 
	// an actual generator expression. Thus we should 
	// not need an Outcome type to wrap any potential 
	// errors, as the body of a generator is always
	// going to succeed. 
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
