#include <string>
#include <memory>

#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"
#include "TypeLiteral.hpp"

TypeLiteral::TypeLiteral(const Location& loc, llvm::Type* literal)
  : Ast(Ast::Kind::TypeLiteral, loc), literal(literal)
{

}

bool TypeLiteral::classof(const Ast* ast)
{
  return ast->GetKind() == Ast::Kind::TypeLiteral;
}

llvm::Type* GetLiteral()
{
  return literal;
}

std::shared_ptr<TypeLiteral> TypeLiteral::Clone()
{
  return std::make_shared<TypeLiteral>(loc, literal);
}

std::string TypeLiteral::ToString()
{
  std::string result;
  llvm::raw_string_ostream handle(result);
  literal->print(handle);
  return handle.str();
}

// we shouldn't really be calling Getype on
// TypeLiteral's.
Judgement TypeLiteral::GetypeV(const Environment& env)
{
  return std::make_shared<TypeLiteral>(loc, literal);
}

// nor should we really be calling Codegen on
// TypeLiterals, though, i suppose being able to
// cast, and compare types in the text of the
// language is useful, so maybe this has a good
// use? putting a pin in this for later...
Judgement TypeLiteral::Codegen(const Environment& env)
{
  return std::make_shared<TypeLiteral>(loc, literal);
}
