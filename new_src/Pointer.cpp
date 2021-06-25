
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Constants.h"

#include "Pointer.hpp"

namespace pink {

/*
  builds an object representing a reference to some value.
  along with it's alignment. this is so we can explicitly
  talk about references in the text of the language, as
  the llvm::Value* itself is a reference to the value
  when we are talking about the code here, implementing
  the compiler.
*/
Pointer::Pointer(const Location& loc, llvm::Value* pointer, llvm::Align alignment)
  : Ast(Ast::Kind::Pointer, loc), pointer(pointer), alignment(alignment)
{

}

/*
  builds a null pointer to the passed type.
*/
Pointer::Pointer(const Location& loc, llvm::PointerType* pointer_type, llvm::Align alignment)
  : Ast(Ast::Kind::Pointer, loc), pointer(llvm::ConstantPointerNull::get(pointer_type)), alignment(alignment)
{

}

std::shared_ptr<Pointer> Pointer::Clone()
{
  return std::make_shared<Pointer>(loc, pointer, alignment);
}

std::string Pointer::ToString()
{
  std::string buf;
  llvm::raw_string_ostream hdl(buf);
  pointer->print(hdl);
  return hdl.str();
}

bool Pointer::classof(const Ast* ast)
{
  return ast->GetKind() == Ast::Kind::Pointer;
}

Judgement Pointer::GetypeV(const Environment& env)
{
  return Judgement(std::make_shared<TypeLiteral>(loc, pointer->getType()));
}

Judgement Pointer::Codegen(const Environment& env)
{
  return Judgement(std::make_shared<ValueLiteral>(loc, pointer));
}

}
