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

Judgement Variable::GetypeV(const Environment& env)
{
  Judgement result;
  std::optional<std::shared_ptr<Ast>> boundval = env.symbols->lookup(name, scope);

  if (boundval)
  {
    result = Judgement((*boundval)->Getype(env));
  }
  else
  {
    result = Judgement(PinkError("Variable [" + std::string(name) + "] not bound in Environment.", loc));
  }

  return result;
}

Judgement Variable::Codegen(const Environment& env)
{
  // return the value bound to this name within the symbols
  // or return an error.
  std::optional<std::shared_ptr<Ast>> boundval = env.symbols->lookup(name, scope);

  if (boundval)
  {
    return Judgement(*boundval);
  }
  else
  {
    return Judgement(PinkError("Variable [" + std::string(name) + "] not bound in Environment.", loc));
  }
}
