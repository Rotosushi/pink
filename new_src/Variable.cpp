#include <string>
#include <memory>
#include <optional>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"
#include "Variable.hpp"

Variable::Variable(const Location& loc, InternedString name, ScopeSet scope)
  : Ast(loc), name(name), scope(scope)
{

}

Variable* Variable::Clone()
{
  return std::make_shared<Variable>(loc, name, scope);
}

std::string Variable::ToString()
{
  return std::string(name);
}

Judgement Variable::Getype(const Environment& env)
{
  Judgement result;
  std::optional<std::shared_ptr<Ast>> boundval = env.symbols->lookup(name, scope);

  if (boundval)
  {
    result = Judgement((*boundval)->Getype(env));
  }
  else
  {
    result = Judgement(PinkError("Unknown Variable: " + std::string(name), loc));
  }

  return result;
}

Judgement Variable::Codegen(const Environment& env)
{
  // generate a load from the local stack, or from a global name.
}
