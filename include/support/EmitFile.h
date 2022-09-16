#pragma once

#include "aux/Environment.h"

namespace pink {
    void EmitObjectFile(const Environment& env, const std::string& filename);
    void EmitAssemblyFile(const Environment& env, const std::string& filename);
    void EmitLLVMFile(const Environment& env, const std::string& filename);
}