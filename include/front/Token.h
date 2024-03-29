// Copyright (C) 2023 cadence
//
// This file is part of pink.
//
// pink is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// pink is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pink.  If not, see <http://www.gnu.org/licenses/>.

/**
 * @file Token.h
 * @brief Header for enum class Token
 * @version 0.1
 *
 */
#pragma once
#include <string_view>

namespace pink {
/**
 * @brief Represents an instance of a Token within the [Lexer](#pink::Lexer) and
 * [Parser](#pink::Parser)
 * 
 * \todo add a bitshift operator 
 * \todo add a divmod operator/builtin
 */
enum class Token : unsigned {
  Error, // an erroneous token
  End,   // EOF

  Id, // [a-zA-Z_][a-zA-Z0-9_]*

  /* Operators */
  Add,                // '+'
  Sub,                // '-'
  Star,               // '*'
  Divide,             // '/'
  Modulo,             // '%'
  And,                // '&'
  Or,                 // '|'
  Not,                // '!'
  Equals,             // '=='
  NotEquals,          // '!='
  LessThan,           // '<'
  LessThanOrEqual,    // '<='
  GreaterThan,        // '>'
  GreaterThanOrEqual, // '>='

  Dot,       // '.'
  Comma,     // ','
  Semicolon, // ';'
  Colon,     // ':'
  ColonEq,   // ':='
  Assign,    // '='
  LParen,    // '('
  RParen,    // ')'
  LBrace,    // '{'
  RBrace,    // '}'
  LBracket,  // '['
  RBracket,  // ']'
  RArrow,    // '->'

  Nil,         // "nil"
  NilType,     // "Nil"
  Integer,     // [0-9]+
  IntegerType, // "Integer"
  True,        // "true"
  False,       // "false"
  BooleanType, // "Boolean"

  Fn,    // 'fn'
  Var,   // 'var'
  If,    // 'if'
  Then,  // 'then'
  Else,  // 'else'
  While, // 'while'
  Do,    // 'do'
};

/**
 * @brief Converts a token into a string representation
 *
 * all this does is uniquely identify the tokens themselves.
 * it does not reproduce the string which was lexed to return
 * a Token::Id, it simply returns "Token::Id".
 * However, if the Token is itself associated with a single unique
 * string, that string will be returned.
 *
 * @param t the token to translate
 * @return std::string the string representing the token
 */
[[maybe_unused]] auto ToString(const Token token) -> std::string_view;
} // namespace pink
