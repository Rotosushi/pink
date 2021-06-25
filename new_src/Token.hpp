#pragma once
#include <string>

enum class Token
{
  // lexical control graphemes
  Error,
  End,

  // language graphemes
  Operator,
  Identifier,
  If,
  Then,
  Else,
  While,
  Do,

  // language symbol graphemes
  Backslash,      // '\'
  Comma,          // ,
  Colon,          // :
  Bind,           // :=
  Body,           // =>
  Assign,         // <-
  FnType,         // ->
  Lparen,         // (
  Rparen,         // )
  Lbrace,         // {
  Rbrace,         // }
  Lbracket,       // [
  Rbracket,       // ]

  // object/value/type graphemes
  Nil,
  Type_Nil,
  Integer,
  Type_Integer,
  True,
  False,
  Type_Boolean,
};

std::string TokenToString(Token token);
