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
#include <string>

namespace pink {
/**
 * @brief Represents an instance of a Token within the [Lexer](#pink::Lexer) and
 * [Parser](#pink::Parser)
 *
 */
enum class Token {
  Error, // an erroneous token
  End,   // EOF

  Id, // [a-zA-Z_][a-zA-Z0-9_]*
  Op, // [+\-*\\%<=>&|\^!~@$]+
  // #TODO replace string_view based operators
  // with token based operators, to remove the lexing
  // ambiguity occring with << and >> when lexing template
  // argument lists.
  // (they must be lexed as '<' '<' and '>' '>' respectively.)
  // (if we add stream insertion or bitwise rotate operators
  //  we will choose non conflicting symbols.
  //  i'm considering '<|' and '|>',  or '<|' and '>|')
  // Add,                // '+'
  // Sub,                // '-'
  // Multiply,           // '*'
  // Divide,             // '/'
  // Modulo,             // '%'
  // And,                // '&'
  // Or,                 // '|'
  // Not,                // '!'
  // Equals,             // '=='
  // NotEquals,          // '!='
  // LessThan,           // '<'
  // LessThanOrEqual,    // '<='
  // GreaterThan,        // '>'
  // GreaterThanOrEqual, // '>='

  Dot,       // '.'
  Comma,     // ','
  Semicolon, // ';'
  Colon,     // ':'
  ColonEq,   // ':='
  Equals,    // '='
  LParen,    // '('
  RParen,    // ')'
  LBrace,    // '{'
  RBrace,    // '}'
  LBracket,  // '['
  RBracket,  // ']'

  Nil,         // "nil"
  NilType,     // "Nil"
  Integer,     // [0-9]+
  IntegerType, // "Integer"
  True,        // "true"
  False,       // "false"
  BooleanType, // "Boolean"
  Pointer,     // "Pointer"
  Slice,       // "Slice"

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
 * a Token::Id, it simply returns "Token::Id". etc...
 *
 * @param t the token to translate
 * @return std::string the string representing the token
 */
[[maybe_unused]] auto TokenToString(const Token token) -> std::string;
} // namespace pink
