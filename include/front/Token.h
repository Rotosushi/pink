#pragma once
#include <string>

namespace pink {
    enum class Token {
        Error,
        End,

        Id,
        Op,

        Colon,
        ColonEq,
        Equals,
        LParen,
        RParen,

        Nil,
        NilType,
        Int,
        IntType,
        True,
        False,
        BoolType,
    };

    std::string TokenToString(Token t);
}
