#pragma once
#include <cstdint> // int8_t


namespace pink {
    enum class Associativity {
        None,
        Left,
        Right,
    };

    typedef int8_t Precedence;
}
