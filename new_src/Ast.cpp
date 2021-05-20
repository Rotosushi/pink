#include "Location.hpp"
#include "TypeLiteral.hpp"
#include "Ast.hpp"

Ast::Ast(const Location& loc) :
  cached_type(nullptr), loc(loc)
{

}

Location Ast::GetLocation()
{
  return loc;
}

Judgement Ast::Getype(const Environment& env)
{
  if (cached_type)
    return Judgement(std::make_shared<TypeLiteral>(cached_type, loc));
  else
  {
    Judgement result = GetypeV(env);

    if (result)
    {
      cached_type = (dynamic_cast<std::shared_ptr<TypeLiteral>>(result.term))->GetLiteral();
    }

    return result;
  }
}
