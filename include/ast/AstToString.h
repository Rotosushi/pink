#pragma once
#include <string>

namespace pink {
class Ast;

auto AstToString(const Ast *ast) -> std::string;
} // namespace pink