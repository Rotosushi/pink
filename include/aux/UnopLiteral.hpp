#pragma once
#include <variant>

#include "llvm/IR/Value.h"

#include "aux/Error.h"
#include "aux/StringInterner.hpp"

#include "ast/Ast.hpp"

#include "type/Type.hpp"

// #TODO Finish This
namespace pink {
    // typedef std::variant<Error, llvm::Value*> (*UnopCodegenFn)(Ast* term, Environment& env);
}
