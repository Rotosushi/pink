
#include "ops/UnopCodegen.h"

namespace pink {
UnopCodegen::UnopCodegen(Type *return_type, UnopCodegenFn gen)
    : result_type(return_type), generate(gen) {}
} // namespace pink
