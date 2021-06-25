#include <vector>
#include <string>
#include <memory>
#include <utility>


#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Argument.hpp"
#include "Ast.hpp"
#include "Object.hpp"
#include "Pointer.hpp"
#include "Closure.hpp"

namespace pink {

Closure::Closure(const Location& loc, Pointer fn_ptr)
  : Ast(Ast::Kind::Closure, loc), function_ptr(fn_ptr), actual_args()
{

}


Closure::Closure(const Location& loc, Pointer fn_ptr, std::vector<Argument>& args)
  : Ast(Ast::Kind::Closure, loc), function_ptr(fn_ptr), actual_args(args)
{

}

bool Closure::classof(const Ast* ast)
{
  return ast->GetKind() == Ast::Kind::Closure;
}

Pointer Closure::GetFunction()
{
  return function_ptr;
}

iterator Closure::begin()
{
  return actual_args.begin();
}

iterator Closure::end()
{
  return actual_args.end();
}

std::shared_ptr<Closure> Closure::Clone()
{
  return std::make_shared<Closure>(loc, function_ptr, actual_args);
}

std::string Closure::ToString()
{
  int i = 0;
  std::string result;
  result += "[";
  for (const Argument& arg : actual_args)
  {
    std::string arg_type_str;
    llvm::raw_string_ostream handle(arg_type_str);
    arg.GetType()->print(handle);

    result += arg.GetName();
    result += ": " + handle.str();

    if (i < (actual_args.size() - 1))
      result += ", ";
  }
  result += "]";
  result += function_ptr.ToString();
  return result;
}


// so, from a typeing perspective we
// want to treat closures like functions,
// they are used in function position,
// and they are being used to represent
// 'stored' functions. however, from a
// codegeneration perspective, the object
// that is a Closure is really an anonymous
// structure, composed of the function pointer,
// plus the tuple of values that are the
// actual arguments.
// thus application terms consume
// closure terms, and actual argument list terms.
// to either build a new closure, or a call term.
Judgement Closure::GetypeV(const Environment& env)
{
  // a closure is a type that is composed of
  // a pink-array of integers. (pink arrays
  // hold their size unlike C arrays.),
  // a function pointer, and a packed struct of
  // applied arguments.
  // essentially: {{i32 = N + 2, [N + 2 x i32]}; fn-ptr; {arg0, arg1, ... argN}
  // where the first slot of the array holds the size of the
  // structure of arguments, and the second slot of the integer array holds the
  // expected number of arguments,  and each member of the
  // array after holds the offset into the structure that starts the element
  // in question. thus element one of the argument list starts
  // at (address-of-the-struct-itself + offset-of-argument-one)
  // and goes to (offset-of-argument-two - offset-of-argument-one)
  // the last element in the list, element N. starts at offset N,
  // and goes to the end of the struct.
  // packed structs have these memory semantics, non-packed
  // structs insert padding into each element such that
  // element access is always correctly aligned.
  // this means GEP's are simpler to implement.
  std::vector<llvm::Type*> closure_type;

  llvm::ArrayType* closure_descriptor_type = llvm::ArrayType

  tuple_type += function_ptr->getType();

  for (const Argument& arg : actual_args)
  {
    tuple_type += arg.GetType();
  }

  llvm::StructType* closure_literal_type = llvm::StructType::get(env.GetContext(), tuple_type);
  // we can still use this tuple type, it just happens to be the type of
  // the held arguments substructure within the closure.

  //return Judgement(std::make_shared<TypeLiteral>(loc, closure_literal_type));
}

// closures are immutable.
// we never resize a closure to support adding more arguments.
// we always build a new closure, in this way the return value
// of procedures returning closures will be a return value of
// a tuple, essentially. except they contain a special field
// which describes the contents of the closure itself, so they
// can be passed in as arguments to other procedures and that
// procedure can process any closure it gets generically.
// processing N held arguments against M expected arguments
// where N is stored within the closure argument itself at runtime.
// and M is known at the time where we are emmiting this particular
// application of the passed closure literal.
Judgement Closure::Codegen(const Environment& env)
{
  // build a llvm::ConstantStruct holding the
  // type descriptor, the function lookup value, and
  // the constant structure holding each held argument.

  /*
    the type descriptor is an array of elements holding
    pointers to each element we want to use as an argument.

    building and storing a function pointer is like

    llvm::Vaue* fnptr = bldr.CreateAlloca(function->getType(), nullptr));
    bldr.CreateStore(function, fnptr, false);
    ...
    llvm::Value* tmp = bldr.CreateLoad(fnptr);
    llvm::CallInst* call = bldr.CreateCall(tmp, actual-args-array);

    i don't think we can use the same call instruction to build
    applications of the function pointer we are retrieving because
    each application could have a different arguments. and i don't
    think the llvm code is building the call term at runtime
    based on the number of arguments it is given, which is
    something that is now runtime dependant. i think this means
    we have to do something different. the call has to be the
    same signature each time, so I guess we want to simply pass the
    structure directly. this could be implemented with a
    memcpy fortunately.
    the return type of each function held within the mapping
    could be different, because we are using the map for all procedures.
    this means that called procedures have a signature as follows
    void(void* return-value, void* args); where the function is implemented such that
    the arguments are extracted out of this opaque structure when we use them within
    the function itself. this opaque passed structure has
    a layout equivalent to the defined argument list of the procedure, simply
    layed out as a structure.
    the caller allocates the return value,

  */

}

Judgement Closure::GetPresentedFunctionType(const Environment& env)
{
  Judgement fn_ptr_jdgmt = function_ptr->Getype(env);

  if (fn_ptr_jdgmt.IsAnError())
    return fn_ptr_jdgmt;

  llvm::Type fn_ptr_type = (dynamic_cast<std::shared_ptr<TypeLiteral>>(fn_ptr_jdgmt.term))->GetLiteral();
  llvm::FunctionType fn_type = dynamic_cast<llvm::FunctionType*>(fn_ptr_type);

  // get our iterators into each of the argument lists.
  llvm::FunctionType::param_iterator fi = fn_type->param_begin();
  std::vector<Argument>::iterator    ai = actual_args.begin();

  // walk along the actual arguments presented
  // and check if the can be bound to each
  // corresponding formal argument.
  while (ai != actual_args.end())
  {
    if (ai->GetType() != (*fi))
    {
      FatalError("Actual argument cannot bind to Formal argument!", __FILE__, __LINE__);
    }

    fi++;
    ai++;
  }

  // we are out of captured arguments, so now we
  // can build the type of the closure itself,
  // from the rest of the function type.
  // if we are also out of formal arguments,
  // this is a full call expression, and will
  // have a type of resulttype.
  if (fi == fn_type->param_end())
    return Judgement(std::make_shared<TypeLiteral>(loc, fn_type->getResultType()));
  else
  {
    // this is a closure construction expression,
    // where we build a new closure holding all of
    // the old arguments, plus the new ones being
    // provided by this application term.
    std::vector<llvm::Type*> presented_argument_types;

    while (fi != fn_type->param_end())
    {
      presented_argument_types += *fi;
      fi++;
    }

    llvm::FunctionType presented_fn_type = llvm::FunctionType::get(fn_type->getResultType(), presented_argument_types, /* is-vararg? */ false);
    return Judgement(std::make_shared<TypeLiteral>(loc, presented_fn_type));
  }
}

}



// --
