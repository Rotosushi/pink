#pragma once
#include <string>

namespace pink {
auto Gensym(const std::string &seed = "") -> std::string;
}