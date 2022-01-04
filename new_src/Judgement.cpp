
#include "Ast.hpp"
#include "Error.hpp"
#include "Judgement.hpp"

namespace pink {

Judgement::Judgement()
  : erroneous(true), error("Default Judgement", Location())
{

}

Judgement::Judgement(Error& err)
  : erroneous(true), error(err)
{

}

Judgement::Judgement(std::shared_ptr<Ast> term)
  : erroneous(false), term(term)
{

}

bool Judgement::operator bool()
{
  return erroneous;
}

Judgement& Judgement::operator=(const Judgement& rhs)
{
  erroneous = rhs.erroneous;
  if (rhs.erroneous)
  {
    error = rhs.error;
  }
  else
  {
    term = rhs.term
  }
  return *this;
}

bool Judgement::IsAnError()
{
  return !erroneous;
}

Error& Judgement::GetError()
{
  if (erroneous == true)
  {
    return error;
  }
  else
  {
    FatalError("Judgement doesn't contain an error", __FILE__, __LINE__);
  }
}

std::shared_ptr<Ast> Judgement::GetTerm()
{
  if (erroneous == false)
  {
    FatalError("Judgement doesn't contain a valid term", __FILE__, __LINE__);
  }
  else
  {
    return term;
  }
}

}