#pragma once
#include <memory>

#include "PinkError.hpp"

class Ast;

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

  Judgement& operator=(const Judgement& rhs);

  bool IsAnError();

  PinkError& GetError();
  std::shared_ptr<Ast>       GetTerm();
};
