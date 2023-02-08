#include "catch2/catch_test_macros.hpp"

#include "front/Token.h"

TEST_CASE("front/Token", "[unit][front]") {
  std::vector<pink::Token> tokens = {
      pink::Token::Error,     pink::Token::End,         pink::Token::Id,
      pink::Token::Op,        pink::Token::Dot,         pink::Token::Comma,
      pink::Token::Semicolon, pink::Token::Colon,       pink::Token::ColonEq,
      pink::Token::Equals,    pink::Token::LParen,      pink::Token::RParen,
      pink::Token::LBrace,    pink::Token::RBrace,      pink::Token::LBracket,
      pink::Token::RBracket,  pink::Token::Nil,         pink::Token::NilType,
      pink::Token::Integer,   pink::Token::IntegerType, pink::Token::True,
      pink::Token::False,     pink::Token::BooleanType, pink::Token::Pointer,
      pink::Token::Fn,        pink::Token::Var,         pink::Token::If,
      pink::Token::Then,      pink::Token::Else,        pink::Token::While,
      pink::Token::Do,
  };
  std::vector<const char *> texts = {
      "Token::Error",     "Token::End",         "Token::Id",
      "Token::Op",        "Token::Dot",         "Token::Comma",
      "Token::Semicolon", "Token::Colon",       "Token::ColonEq",
      "Token::Equals",    "Token::LParen",      "Token::RParen",
      "Token::LBrace",    "Token::RBrace",      "Token::LBracket",
      "Token::RBracket",  "Token::Nil",         "Token::NilType",
      "Token::Integer",   "Token::IntegerType", "Token::True",
      "Token::False",     "Token::BooleanType", "Token::Pointer",
      "Token::Fn",        "Token::Var",         "Token::If",
      "Token::Then",      "Token::Else",        "Token::While",
      "Token::Do",
  };
  size_t index = 0;
  for (const auto &token : tokens) {
    REQUIRE(TokenToString(token) == texts[index]);
    index++;
  }
}