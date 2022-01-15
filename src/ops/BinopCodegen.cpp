#include "ops/BinopCodegen.h"



namespace pink {
    BinopCodegen::BinopCodegen(const BinopCodegen& other)
        : result_type(other.result_type), generate(other.generate)
    {

    }

    BinopCodegen::BinopCodegen(Type* ret_t, BinopCodegenFn fn)
        : result_type(ret_t), generate(fn)
    {

    }
}























// --
