#pragma once


namespace pink {
    enum class Token {
        Error,
        End,

        Id,
        Op,

        Colon,
        Assign,
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
}
