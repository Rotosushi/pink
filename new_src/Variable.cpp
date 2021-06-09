#include <string>
#include <memory>
#include <optional>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"
#include "Variable.hpp"

Variable::Variable(const Location& loc, InternedString name, ScopeSet scope)
  : Ast(Ast::Kind::Variable, loc), name(name), scope(scope)
{

}

bool Variable::classof(const Ast* ast)
{
  return ast->GetKind() == Ast::Kind::Variable;
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
  std::optional<llvm::AllocaInst*> boundval = env.symbols->lookup(name, scope);

  if (boundval)
  {
    // the symboltable we maintain keeps track of
    // local allocations that we want to utilize.
    // the type of the variable is it's refferent type.
    result = Judgement((*boundval)->getAllocatedType(env));
  }
  else
  {
    result = Judgement(PinkError("Variable [" + std::string(name) + "] not bound in Environment.", loc));
  }

  return result;
}

// if we change the usage of this procedure to
// always return a reference to the allocation
// via a pointer. then in the user code would
// always need to contend with the by ref
// nature of the code. however, if we compute
// a Load first, then we only return a pointer
// if we are talking about a huge allocation.
// and that restricts where other program code
// has to deal explicitly with large allocations.
Judgement Variable::Codegen(const Environment& env)
{
  // return the value bound to this name within the SymbolTable
  // or return an error.
  std::optional<llvm::AllocaInst*> bound = env.symbols->lookup(name, scope);

  if (bound)
  {
    // this should return a pointer to the location, and we get to
    // treat the thing returned from a name as a pointer.
    // we will now have to delegate the loading of the variable to
    // the user of the variable.
    return Judgement(std::make_shared<ValueLiteral>(loc, llvm::cast<llvm::Value>(*bound)));
  }
  else
  {
    return Judgement(PinkError("Variable [" + std::string(name) + "] not bound in Environment.", loc));
  }
}
