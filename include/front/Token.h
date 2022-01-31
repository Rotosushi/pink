#pragma once
#include <string>

namespace pink {
    enum class Token {
        Error,
        End,

        Id,
        Op,

		Semicolon,
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
    bool TokenToBool(Token t);
}
