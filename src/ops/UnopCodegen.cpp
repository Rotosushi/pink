
#include "ops/UnopCodegen.hpp"

namespace pink {
    UnopCodegen::UnopCodegen(const UnopCodegen& other)
        : result_type(other.result_type), generate(other.generate)
    {

    }

    UnopCodegen::UnopCodegen(Type* rt, UnopCodegenFn gen)
        : result_type(rt), generate(gen)
    {

    }
}
