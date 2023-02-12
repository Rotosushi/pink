#include "catch2/catch_test_macros.hpp"

#include "front/Lexer.h"

#include <cstring>

TEST_CASE("front/Lexer", "[unit][front]") {
  std::vector<const char *> source_lines = {
      "symbol\n", "+\n",     "108\n",     "nil\n", "Nil\n", "Integer\n",
      "true\n",   "false\n", "Boolean\n", "fn\n",  "if\n",  "then\n",
      "else\n",   "while\n", "do\n",      ".\n",   ",\n",   ";\n",
      ":\n",      "=\n",     ":=\n",      "(\n",   ")\n",   "{\n",
      "}\n",      "[\n",     "]\n",
  };

  std::vector<pink::Token> equivalent_tokens = {
      pink::Token::Id,     pink::Token::Op,       pink::Token::Integer,
      pink::Token::Nil,    pink::Token::NilType,  pink::Token::IntegerType,
      pink::Token::True,   pink::Token::False,    pink::Token::BooleanType,
      pink::Token::Fn,     pink::Token::If,       pink::Token::Then,
      pink::Token::Else,   pink::Token::While,    pink::Token::Do,
      pink::Token::Dot,    pink::Token::Comma,    pink::Token::Semicolon,
      pink::Token::Colon,  pink::Token::Equals,   pink::Token::ColonEq,
      pink::Token::LParen, pink::Token::RParen,   pink::Token::LBrace,
      pink::Token::RBrace, pink::Token::LBracket, pink::Token::RBracket,
  };

  auto [test_text, source_locations] = [&source_lines]() {
    size_t                      line_number = 0;
    std::string                 test_text;
    std::vector<pink::Location> source_locations;

    for (auto &line : source_lines) {
      line_number += 1;
      source_locations.emplace_back(line_number,
                                    0,
                                    line_number,
                                    strlen(line) - 1);
      test_text += line;
    }

    return std::make_pair(test_text, source_locations);
  }();

  pink::Lexer lexer(test_text);

  auto location_cursor = source_locations.begin();
  auto location_end    = source_locations.end();
  auto token_cursor    = equivalent_tokens.begin();
  auto token_end       = equivalent_tokens.end();
  while ((location_cursor != location_end) && (token_cursor != token_end)) {
    auto &location = *location_cursor;
    auto  token    = *token_cursor;

    auto lexed_token    = lexer.lex();
    auto lexed_location = lexer.loc();

    REQUIRE(lexed_token == token);
    REQUIRE(lexed_location == location);

    location_cursor++;
    token_cursor++;
  }
}