#pragma once
#include <memory>
#include <string>

#include "Location.hpp"
#include "Token.hpp"
#include "Environment.hpp"

/*
  an re2c implemented lexer.

  the lexer does not assume ownership of the lexed buffer.
*/

namespace pink {

class Lexer
{
private:
  const Environment& env;
  Location loc;
  std::string& buffer;
  std::string::iterator end;
  std::string::iterator cursor;
  std::string::iterator marker;
  std::string::iterator token;

  void update_loc();
public:
  Lexer(const Environment& env);
  Lexer(const std::string& data, const Environment& env);

  void SetBuffer(const std::string& data);
  void Reset();

  // these names are not camelcase for historical reasons.
  // don't @ me. Yylex just looks weird
  Token       yylex();
  std::string yytxt();
  Location    yyloc();
};

}
