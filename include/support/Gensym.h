#pragma once
#include <string>

namespace pink {
/**
 * @brief generates a new symbol, appends a counter value 
 * of the number of times Gensym has been called. 
 * 
 * @param prefix 
 * @return std::string 
 */
auto Gensym(const std::string &prefix = "") -> std::string;
}