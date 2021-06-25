#pragma once
#include <string>
#include <memory>

#include "Environment.hpp"
#include "Location.hpp"
#include "Judgement.hpp"
#include "Token.hpp"
#include "Lexer.hpp"

/*
  a (mostly) LL(1) parser.
  handrolled because I can't be bothered
  to make bison work with c++ and re2c
  and user defined operators.
  LL(1) because then the structure
  of the grammar rhymes with the
  structure of the rest of the definition
  of the language.

  the parser does not assume ownership of the passed buffer.
*/

namespace pink {

class Parser
{
private:
  // parsing data
  const Environment& env;
  Lexer lexer;

  Token       curtok;
  Location    curloc;
  std::string curtxt;

  // parsing primitives
  void NextToken();
  bool IsPrimary(Token token);
  bool IsType(Token token);
  bool IsEnding(Token token);
  InternedString IsBinop(std::string& optxt);
  InternedString IsUnop(std::string& optxt);

  // the true parsing functions.
  Judgement ParseTerm();
  Judgement ParseAffix();
  Judgement ParseApplication();
  Judgement ParseInfix(std::shared_ptr<Ast> l, unsigned min_precedence);
  Judgement ParsePrimary();
  Judgement ParseLambda();
  //Judgement ParseConditional(const Environment& env);
  //Judgement ParseIteration(const Environment& env);


public:
  Parser(const Environment& env);

  void Reset();

  Judgement Parse(const std::string& data, const Environment& env);

  // Judgement ParseFile(const std::string& filename, const Environment& env); ??
  // this is a maybe, the parser may not be the best place to handle
  // this distinction between file and REPL most effectively.
  // it may be easier to consider parsing each line we
  // read from the file, instead of parsing whole files with one call
};

}
