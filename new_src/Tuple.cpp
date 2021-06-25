#include "Tuple.hpp"

namespace pink {

Tuple::Tuple(const Location& loc, std::initializer_list<std::shared_ptr<Ast>> values)
  : Object(loc), elements(values)
{

}

Tuple::Tuple(const Location& loc, std::vector<std::shared_ptr<Ast>> values)
  : Object(loc), elements(values)
{

}

iterator Tuple::begin()
{
  return elements.begin();
}

iterator Tuple::end()
{
  return elements.end();
}

std::shared_ptr<Tuple> Tuple::Clone()
{
  std::vector<std::shared_ptr<Ast>> new_elems;
  for (auto& elem : elements)
  {
    new_elems += elem->Clone();
  }
  return std::make_shared<Tuple>(loc, new_elems);
}

std::string Tuple::ToString()
{
  std::size_t i = 0;
  std::string result;
  result += "(";
  for (auto& elem : elements)
  {
    result += elem->ToString();

    if (i < (elements.size() - 1))
      result += ", ";

    i++;
  }
  result += ")";
  return result;
}

Judgement Tuple::GetypeV(const Environment& env)
{
  std::vector<llvm::Type*> agg_type;
  for (auto& elem : elements)
  {
    Judgement elem_jdgmt = elem->Getype(env);

    if (elem_jdgmt.IsAnError())
      return elem_jdgmt;

    std::shared_ptr<TypeLiteral> elem_typeliteral = (dynamic_cast<TypeLiteral>(elem_jdgmt.term))->GetLiteral();

    llvm::Type* elem_type = elem_typeliteral->GetLiteral();

    agg_type += elem_type;
  }

  llvm::Type* tuple_type = llvm::StructType::get(env.GetContext(), agg_type);
  return Judgement(std::make_shared<TypeLiteral>(loc, tuple_type));
}

Judgement Tuple::Codgen(const Environment& env)
{
  // build a constant structure which holds
  // each of the values that make up the
  // tuples elements. each value returned
  // by Codegen has to be a llvm::Constant
  // for us to be able to build a Constant
  // struct.
  std::vector<llvm::Constant*> tuple_elements;

  for (const std::shared_ptr<Ast>& elem : elements)
  {
    Judgement elem_jdgmt = elem->Codegen(env);

    if (elem_jdgmt.IsAnError())
      return elem_jdgmt;

    std::shared_ptr<ValueLiteral> valueliteral = dynamic_cast<std::shared_ptr<ValueLiteral>>(elem_jdgmt.term);
    llvm::Value*    v = valueliteral->GetLiteral();
    llvm::Type*     t = v->getType();
    llvm::Constant* c = nullptr;

    if (llvm::PointerType* pt = llvm::dyn_cast<llvm::PointerType>(t))
    {
      // the value is a reference type, so we unwrap it once.
      c = env.BuildLoad(v);
    }
    else
    {
      // the value is not a Reference type.
      c = llvm::dyn_cast<llvm::Constant>(v);
    }


    if (!c)
    {
      std::string buf;
      llvm::raw_string_ostream hdl(buf);
      v->print(hdl);

      Judgement(Error("Tuple element llvm::Value* [" + std::string(hdl->str() + "] must be a llvm::Constant*", loc)));
    }
    else
    {
      tuple_elements += c;
    }
  }

  llvm::ConstantStruct* tuple = llvm::ConstantStruct::getAnon(tuple_elements);
  return Judgement (std::make_shared<ValueLiteral> (loc, tuple));
}

}

// --
