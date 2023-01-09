#include <cassert>
#include <cstring>
#include <vector>

#include "Test.h"
#include "front/Lexer.h"
#include "front/TestLexer.h"

/*
    Test that the lexer validly lexes the provided
    token, that the provided textual location matches
    the lexed one, and that the lexed string matches the
    input.
*/
auto TestForToken(std::ostream &out, pink::Lexer &lexer, pink::Token token,
                  pink::Location &location) -> bool {
  bool result = true;

  std::string tokstr = TokenToString(token);

  auto current_token = lexer.yylex();
  auto current_location = lexer.yyloc();

  result &= Test(out, std::string("Lexer::yylex(), " + tokstr),
                 current_token == token);
  result &= Test(out, std::string("Lexer::yyloc(), " + tokstr),
                 current_location == location);

  return result;
}

auto TestLexer(std::ostream &out) -> bool {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing Pink::Lexer: \n";

  std::vector<std::string> source_lines = {
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
    size_t line_number = 0;
    std::string test_text;
    std::vector<pink::Location> source_locations;

    for (auto &line : source_lines) {
      line_number += 1;
      source_locations.emplace_back(line_number, 0, line_number,
                                    line.length() - 1);
      test_text += line;
    }

    return std::make_pair(test_text, source_locations);
  }();

  pink::Lexer lexer(test_text);

  auto location_cursor = source_locations.begin();
  auto location_end = source_locations.end();
  assert(location_cursor != location_end);

  for (auto &token : equivalent_tokens) {
    auto location = *location_cursor;

    result &= TestForToken(out, lexer, token, location);

    location_cursor++;
    if (location_cursor == location_end) {
      break;
    }
  }

  result &= Test(out, "pink::Lexer", result);

  out << "\n-----------------------\n";
  return result;
}
