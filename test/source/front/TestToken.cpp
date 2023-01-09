#include "front/TestToken.h"
#include "Test.h"
#include "front/Token.h"

bool Testlet(std::string txt, pink::Token tok) {
  bool result = true;

  if (txt != pink::TokenToString(tok))
    result = false;

  return result;
}

bool TestToken(std::ostream &out) {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::Token: \n";

  result &=
      Test(out, "Token::Error", Testlet("Token::Error", pink::Token::Error));
  result &= Test(out, "Token::End", Testlet("Token::End", pink::Token::End));
  result &= Test(out, "Token::Id", Testlet("Token::Id", pink::Token::Id));
  result &=
      Test(out, "Token::FullyQualifiedId",
           Testlet("Token::FullyQualifiedId", pink::Token::FullyQualifiedId));
  result &= Test(out, "Token::Op", Testlet("Token::Op", pink::Token::Op));
  result &= Test(out, "Token::Dot", Testlet("Token::Dot", pink::Token::Dot));
  result &=
      Test(out, "Token::Comma", Testlet("Token::Comma", pink::Token::Comma));
  result &=
      Test(out, "Token::Colon", Testlet("Token::Colon", pink::Token::Colon));
  result &= Test(out, "Token::ColonEq",
                 Testlet("Token::ColonEq", pink::Token::ColonEq));
  result &= Test(out, "Token::Semicolon",
                 Testlet("Token::Semicolon", pink::Token::Semicolon));
  result &=
      Test(out, "Token::Equals", Testlet("Token::Equals", pink::Token::Equals));
  result &=
      Test(out, "Token::LParen", Testlet("Token::LParen", pink::Token::LParen));
  result &=
      Test(out, "Token::RParen", Testlet("Token::RParen", pink::Token::RParen));
  result &=
      Test(out, "Token::LBrace", Testlet("Token::LBrace", pink::Token::LBrace));
  result &=
      Test(out, "Token::RBrace", Testlet("Token::RBrace", pink::Token::RBrace));
  result &= Test(out, "Token::LBracket",
                 Testlet("Token::LBracket", pink::Token::LBracket));
  result &= Test(out, "Token::RBracket",
                 Testlet("Token::RBracket", pink::Token::RBracket));
  result &= Test(out, "Token::Nil", Testlet("Token::Nil", pink::Token::Nil));
  result &= Test(out, "Token::NilType",
                 Testlet("Token::NilType", pink::Token::NilType));
  result &= Test(out, "Token::Integer",
                 Testlet("Token::Integer", pink::Token::Integer));
  result &= Test(out, "Token::IntegerType",
                 Testlet("Token::IntegerType", pink::Token::IntegerType));
  result &= Test(out, "Token::True", Testlet("Token::True", pink::Token::True));
  result &=
      Test(out, "Token::False", Testlet("Token::False", pink::Token::False));
  result &= Test(out, "Token::BooleanType",
                 Testlet("Token::BooleanType", pink::Token::BooleanType));
  result &= Test(out, "Token::Fn", Testlet("Token::Fn", pink::Token::Fn));
  result &= Test(out, "Token::If", Testlet("Token::If", pink::Token::If));
  result &= Test(out, "Token::Then", Testlet("Token::Then", pink::Token::Then));
  result &= Test(out, "Token::Else", Testlet("Token::Else", pink::Token::Else));
  result &=
      Test(out, "Token::While", Testlet("Token::While", pink::Token::While));
  result &= Test(out, "Token::Do", Testlet("Token::Do", pink::Token::Do));

  result &= Test(out, "pink::Token", result);
  out << "\n-----------------------\n";
  return result;
}
