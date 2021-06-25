#include <string>
#include <memory>
#include <optional>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"
#include "Variable.hpp"

namespace pink {

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
    result = Judgement(Error("Variable [" + std::string(name) + "] not bound in Environment.", loc));
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
    // our table binds alloca's, because all local names
    // must be stored on the stack to be assignable.
    // so we first emit a load to convert the alloca ponter
    // to a constant value holding the data from the alloca.
    // for simple types this means loading the cell into a
    // register for working on. for larger values we have to
    // talk about multiple loads.
    // to simplify every location which needs to load values
    // we defined a recursive load procedure which should handle
    // any composite made of structs and arrays. unions are just
    // their largest struct. and are handled as such.

    // For now, See the Assignment.hpp file for more details
    // on why, but we are returning the address of the local
    // allocation when we encounter a variable name now,
    // TL;DR to implement Assignment as a true binary operator.
    //llvm::Constant* loaded_value = env.BuildLoad((*bound));
    return Judgement(std::make_shared<ValueLiteral>(loc, llvm::cast<Value>(*bound)));
  }
  else
  {
    return Judgement(Error("Variable [" + std::string(name) + "] not bound in Environment.", loc));
  }
}

}
