#pragma once
#include <string>

namespace pink {
    /**
     * @brief Represents an instance of a Token within the [Lexer](#Lexer) and [Parser](#Parser)
     * 
     */
    enum class Token {
        Error, // an erroneous token
        End, // EOF

        Id,  // [a-zA-Z_][a-zA-Z0-9_]*
        FullyQualifiedId, // Id (:: Id)+
        Op,  // [+\-*\\%<=>&|\^!~@$]+

        Dot,       // '.'
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
        Ptr,     // "ptr"
                
        Fn,      // 'fn'
        Var,     // 'var'
        If,      // 'if'
        Then,    // 'then'
        Else,    // 'else'
        While,   // 'while'
        Do,      // 'do'
    };

    /**
     * @brief Converts a token into a string representation
     * 
     * all this does is uniquely identify the tokens themselves.
     * it does not reproduce the string which was lexed to return 
     * a Token::Id, it simply returns "Token::Id". etc.
     * 
     * @param t the token to translate
     * @return std::string the string representing the token
     */
    std::string TokenToString(Token t);

    /**
     * @brief converts a Token to a boolean value
     * 
     * This is necessary only to allow Token literals to be present 
     * within conditional expressions.
     * 
     * @param t the Token to convert
     * @return true if t *is not* Token::Error
     * @return false if t *is* Token::Error
     */
    bool TokenToBool(Token t);
}
