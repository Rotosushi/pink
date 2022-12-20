#include <cstring>

#include "Test.h"
#include "front/Lexer.h"
#include "front/TestLexer.h"

/*
    Test that the lexer validly lexes the provided
    token, that the provided textual location matches
    the lexed one, and that the lexed string matches the
    input.
*/
bool TestForToken(std::ostream &out, pink::Lexer &lex, pink::Token t,
                  pink::Location &l) {
  bool result = true;

  std::string tokstr = TokenToString(t);

  pink::Token tok = lex.yylex();
  pink::Location loc = lex.yyloc();

  result &= Test(out, std::string("Lexer::yylex(), " + tokstr), tok == t);
  result &= Test(out, std::string("Lexer::yyloc(), " + tokstr), loc == l);

  return result;
}

bool TestLexer(std::ostream &out) {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing Pink::Lexer: \n";

  pink::Location loc;
  std::string buf = "symbol\n";     // line 1
  buf += "global::local::symbol\n"; // line 2
  buf += "+\n";                     // line 3
  buf += "108\n";                   // line 4
  buf += "nil\n";                   // line 5
  buf += "Nil\n";                   // line 6
  buf += "Int\n";                   // line 7
  buf += "true\n";                  // line 8
  buf += "false\n";                 // line 9
  buf += "Bool\n";                  // line 10

  pink::Lexer lex(buf);

  // symbol
  loc = {1, 0, 1, 6};
  result &= TestForToken(out, lex, pink::Token::Id, loc);

  // global::local::symbol
  loc = {2, 0, 2, 21};
  result &= TestForToken(out, lex, pink::Token::FullyQualifiedId, loc);

  // +
  loc = {3, 0, 3, 1};
  result &= TestForToken(out, lex, pink::Token::Op, loc);

  // 108
  loc = {4, 0, 4, 3};
  result &= TestForToken(out, lex, pink::Token::Int, loc);

  // nil
  loc = {5, 0, 5, 3};
  result &= TestForToken(out, lex, pink::Token::Nil, loc);

  // Nil
  loc = {6, 0, 6, 3};
  result &= TestForToken(out, lex, pink::Token::NilType, loc);

  // Int
  loc = {7, 0, 7, 3};
  result &= TestForToken(out, lex, pink::Token::IntegerType, loc);

  // true
  loc = {8, 0, 8, 4};
  result &= TestForToken(out, lex, pink::Token::True, loc);

  // false
  loc = {9, 0, 9, 5};
  result &= TestForToken(out, lex, pink::Token::False, loc);

  // Bool
  loc = {10, 0, 10, 4};
  result &= TestForToken(out, lex, pink::Token::BooleanType, loc);

  // End
  loc = {11, 0, 11, 0};
  result &= TestForToken(out, lex, pink::Token::End, loc);

  lex.AppendBuf("fn\n");    // line 11
  lex.AppendBuf("if\n");    // line 12
  lex.AppendBuf("then\n");  // line 13
  lex.AppendBuf("else\n");  // line 14
  lex.AppendBuf("while\n"); // line 15
  lex.AppendBuf("do\n");    // line 16

  // fn
  loc = {11, 0, 11, 2};
  result &= TestForToken(out, lex, pink::Token::Fn, loc);

  // if
  loc = {12, 0, 12, 2};
  result &= TestForToken(out, lex, pink::Token::If, loc);

  // then
  loc = {13, 0, 13, 4};
  result &= TestForToken(out, lex, pink::Token::Then, loc);

  // else
  loc = {14, 0, 14, 4};
  result &= TestForToken(out, lex, pink::Token::Else, loc);

  // while
  loc = {15, 0, 15, 5};
  result &= TestForToken(out, lex, pink::Token::While, loc);

  // do
  loc = {16, 0, 16, 2};
  result &= TestForToken(out, lex, pink::Token::Do, loc);

  lex.AppendBuf(".\n");
  lex.AppendBuf(",\n");
  lex.AppendBuf(";\n");
  lex.AppendBuf(":\n");
  lex.AppendBuf("=\n");
  lex.AppendBuf(":=\n");
  lex.AppendBuf("(\n");
  lex.AppendBuf(")\n");
  lex.AppendBuf("{\n");
  lex.AppendBuf("}\n");
  lex.AppendBuf("[\n");
  lex.AppendBuf("]\n");

  // .
  loc = {17, 0, 17, 1};
  result &= TestForToken(out, lex, pink::Token::Dot, loc);

  // ,
  loc = {18, 0, 18, 1};
  result &= TestForToken(out, lex, pink::Token::Comma, loc);

  // ;
  loc = {19, 0, 19, 1};
  result &= TestForToken(out, lex, pink::Token::Semicolon, loc);

  // :
  loc = {20, 0, 20, 1};
  result &= TestForToken(out, lex, pink::Token::Colon, loc);

  // =
  loc = {21, 0, 21, 1};
  result &= TestForToken(out, lex, pink::Token::Equals, loc);

  // :=
  loc = {22, 0, 22, 2};
  result &= TestForToken(out, lex, pink::Token::ColonEq, loc);

  // (
  loc = {23, 0, 23, 1};
  result &= TestForToken(out, lex, pink::Token::LParen, loc);

  // )
  loc = {24, 0, 24, 1};
  result &= TestForToken(out, lex, pink::Token::RParen, loc);

  // {
  loc = {25, 0, 25, 1};
  result &= TestForToken(out, lex, pink::Token::LBrace, loc);

  // }
  loc = {26, 0, 26, 1};
  result &= TestForToken(out, lex, pink::Token::RBrace, loc);

  // [
  loc = {27, 0, 27, 1};
  result &= TestForToken(out, lex, pink::Token::LBracket, loc);

  // ]
  loc = {28, 0, 28, 1};
  result &= TestForToken(out, lex, pink::Token::RBracket, loc);

  result &= Test(out, "pink::Lexer", result);

  out << "\n-----------------------\n";
  return result;
}
