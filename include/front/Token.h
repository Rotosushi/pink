#pragma once
#include <string>

namespace pink {
    enum class Token {
        Error,
        End, // EOF

        Id,  // [a-zA-Z_][a-zA-Z0-9_]*
        Op,  // [+\-*\\%<=>&|\^!~@$]+

        Comma,     // ','
		    Semicolon, // ';'
        Colon,     // ':'
        ColonEq,   // ':='
        Equals,    // '='
        LParen,    // '('
        RParen,    // ')'
        LBrace,	   // '{'
        RBrace,    // '}'
        LBracket,  // '['
        RBracket,  // ']'

        Nil,     // "nil"
        NilType, // "Nil"
        Int,     // [0-9]+
        IntType, // "Int"
        True,    // "true"
        False,   // "false"
        BoolType,// "Bool"
                
        Fn,      // 'fn'
        If,      // 'if'
        Then,    // 'then'
        Else,    // 'else'
    };

    std::string TokenToString(Token t);
    bool TokenToBool(Token t);
}
