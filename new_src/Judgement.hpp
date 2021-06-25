#pragma once
#include <memory>

#include "Error.hpp"

namespace pink {

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
    Error            error;
    std::shared_ptr<Ast> term;
  };
public:
  Judgement();
  Judgement(Error& err);
  Judgement(std::shared_ptr<Ast> term);

  bool       operatorbool();
  Judgement& operator=(const Judgement& rhs);

  bool IsAnError();

  Error& GetError();
  std::shared_ptr<Ast> GetTerm();
};

}
