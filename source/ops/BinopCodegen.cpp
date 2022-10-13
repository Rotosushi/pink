#include "ops/BinopCodegen.h"

namespace pink {
BinopCodegen::BinopCodegen(Type *ret_t, BinopCodegenFn fn_p)
    : result_type(ret_t), generate(fn_p) {}
} // namespace pink

// --
