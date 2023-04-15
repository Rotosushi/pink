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

#include "catch2/catch_test_macros.hpp"

#include "front/Token.h"

TEST_CASE("front/Token", "[unit][front]") {
  std::vector<pink::Token> tokens = {
      pink::Token::Error,
      pink::Token::End,
      pink::Token::Id,
      pink::Token::Add,
      pink::Token::Sub,
      pink::Token::Star,
      pink::Token::Divide,
      pink::Token::Modulo,
      pink::Token::And,
      pink::Token::Or,
      pink::Token::Not,
      pink::Token::Equals,
      pink::Token::NotEquals,
      pink::Token::LessThan,
      pink::Token::LessThanOrEqual,
      pink::Token::GreaterThan,
      pink::Token::GreaterThanOrEqual,
      pink::Token::Dot,
      pink::Token::Comma,
      pink::Token::Semicolon,
      pink::Token::Colon,
      pink::Token::ColonEq,
      pink::Token::Assign,
      pink::Token::LParen,
      pink::Token::RParen,
      pink::Token::LBrace,
      pink::Token::RBrace,
      pink::Token::LBracket,
      pink::Token::RBracket,
      pink::Token::Nil,
      pink::Token::NilType,
      pink::Token::Integer,
      pink::Token::IntegerType,
      pink::Token::True,
      pink::Token::False,
      pink::Token::BooleanType,
      pink::Token::Fn,
      pink::Token::Var,
      pink::Token::If,
      pink::Token::Then,
      pink::Token::Else,
      pink::Token::While,
      pink::Token::Do,
  };
  std::vector<const char *> texts = {
      "Token::Error",
      "Token::End",
      "Token::Id",
      "+",
      "-",
      "*",
      "/",
      "%",
      "&",
      "|",
      "!",
      "==",
      "!=",
      "<",
      "<=",
      ">",
      ">=",
      ".",
      ",",
      ";",
      ":",
      ":=",
      "=",
      "(",
      ")",
      "{",
      "}",
      "[",
      "]",
      "nil",
      "Nil",
      "Token::Integer",
      "Integer",
      "true",
      "false",
      "Boolean",
      "fn",
      "var",
      "if",
      "then",
      "else",
      "while",
      "do",
  };
  size_t index = 0;
  for (const auto &token : tokens) {
    CHECK(ToString(token) == texts[index]);
    index++;
  }
}