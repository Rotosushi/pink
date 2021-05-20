#pragma once
#include <memory>

#include "PinkError.hpp"

class Ast;


/*
  Judgement encapsulates the two major paths of
  our algorithm, either we are building a correct
  code/type, or we are building a description of
  the error we encountered.
*/
class Judgement
{
private:
  bool erroneous;
  union {
    PinkError            error;
    std::shared_ptr<Ast> term;
  };
public:
  Judgement();
  Judgement(PinkError& err);
  Judgement(std::shared_ptr<Ast> term);

  bool       operatorbool();
  Judgement& operator=(const Judgement& rhs);

  bool IsAnError();

  PinkError& GetError();
  std::shared_ptr<Ast> GetTerm();
};
