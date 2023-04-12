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

#include "front/Token.h"

#include "support/FatalError.h"

namespace pink {
/*

*/
auto ToString(const Token token) -> std::string_view {
  switch (token) {
  case Token::Error: {
    return "Token::Error";
  }
  case Token::End: {
    return "Token::End";
  }

  case Token::Id: {
    return "Token::Id";
  }

  case Token::Add: {
    return "+";
  }
  case Token::Sub: {
    return "-";
  }
  case Token::Star: {
    return "*";
  }
  case Token::Divide: {
    return "/";
  }
  case Token::Modulo: {
    return "%";
  }
  case Token::And: {
    return "&";
  }
  case Token::Or: {
    return "|";
  }
  case Token::Not: {
    return "!";
  }
  case Token::Equals: {
    return "==";
  }
  case Token::NotEquals: {
    return "!=";
  }
  case Token::LessThan: {
    return "<";
  }
  case Token::LessThanOrEqual: {
    return "<=";
  }
  case Token::GreaterThan: {
    return ">";
  }
  case Token::GreaterThanOrEqual: {
    return ">=";
  }

  case Token::Dot: {
    return ".";
  }
  case Token::Comma: {
    return ",";
  }
  case Token::Semicolon: {
    return ";";
  }
  case Token::Colon: {
    return ":";
  }
  case Token::Assign: {
    return "=";
  }
  case Token::ColonEq: {
    return ":=";
  }
  case Token::LParen: {
    return "(";
  }
  case Token::RParen: {
    return ")";
  }
  case Token::LBrace: {
    return "{";
  }
  case Token::RBrace: {
    return "}";
  }
  case Token::LBracket: {
    return "[";
  }
  case Token::RBracket: {
    return "]";
  }

  case Token::Nil: {
    return "nil";
  }
  case Token::NilType: {
    return "Nil";
  }
  case Token::Integer: {
    return "Token::Integer";
  }
  case Token::IntegerType: {
    return "Integer";
  }
  case Token::True: {
    return "true";
  }
  case Token::False: {
    return "false";
  }
  case Token::BooleanType: {
    return "Boolean";
  }
  case Token::Pointer: {
    return "Pointer";
  }
  case Token::Slice: {
    return "Slice";
  }

  case Token::Fn: {
    return "fn";
  }
  case Token::Var: {
    return "var";
  }
  case Token::If: {
    return "if";
  }
  case Token::Then: {
    return "then";
  }
  case Token::Else: {
    return "else";
  }
  case Token::While: {
    return "while";
  }
  case Token::Do: {
    return "do";
  }
  default: {
    FatalError("Unknown Token Kind");
    return {"Unknown"};
  }
  }
}
} // namespace pink
