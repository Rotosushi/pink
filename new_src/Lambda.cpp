
#include <vector>
#include <utility>
#include <string>
#include <memory>

#include "llvm/Support/Casting.h"

#include "llvm/IR/Verifier.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Argument.h"

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Argument.hpp"
#include "Object.hpp"
#include "Lambda.hpp"

namespace pink {

Lambda::Lambda(const Location& loc, std::vector<Argument>& formal_args, SymbolTable* outer_symbols, std::shared_ptr<Ast> body)
  : Ast(Ast::Kind::Lambda, loc),
    lambda_attributes(),
    formal_args(formal_args),
    symbols(outer_symbols),
    body(body)
{

}

bool Lambda::classof(const Ast* ast)
{
  return ast->GetKind() == Ast::Kind::Lambda;
}

std::shared_ptr<Lambda> Lambda::Clone()
{
  return std::make_shared<Lambda>(loc, formal_args, symbols->GetOuterScope(), body->Clone());
}

virtual std::string Lambda::ToString()
{
  std::string result;
  result += "\\";
  for (std::size_t i = 0; i < formal_args.length(); i++)
  {
    const Argument& arg = formal_args[i];

    std::string buf;
    llvm::raw_string_ostream arg_type_str(buf);
    arg.GetType()->print(arg_type_str);

    result += arg.GetName();
    result += ": " arg_type_str.str();

    if (i < (formal_args.length() - 1))
    {
      result += ", ";
    }
  }
  result += " => " + body->ToString();
  return result;
}


/*
  computes the formal type of the procedure
  this lambda represents.
*/
Judgement Lambda::GetypeV(const Environment& env)
{
  // we now need to interface with llvm and that
  // has implications on the shape of this code.
  // - non-reference arguments that are smaller than
  //  or equal to in size to a single word can be
  //  unannotated. (or given inreg, but llvm says
  //  that is target specific so maybe that makes it
  //  hared down the line to implement potability? idk)
  // - non-reference arguments that are larger
  //  in size than a single word need to be annotated byvalue.
  //  - and byvalue parameters need to be converted to
  //    pointers to the type of the parameter.
  Location dummy_location;
  // we need a new environment, with the Lambda's
  // symboltable bound instead of the outer symboltable
  // during codegen we also want to change the IRBuilder's
  // insertion point to the functions basic block
  // before we emit it's body, but typechecking just needs to maintain
  // bindings to types.
  Environment lambda_env(env.GetInternedNames(),
                         env.GetInternedOperators(),
                         *this->symbols,
                         env.GetBinopTable(),
                         env.GetUnopTable(),
                         env.GetTargetTriple(),
                         env.GetDataLayout(),
                         env.GetContext(),
                         env.GetModule(),
                         env.GetIRBuilder());
  /*
    so, the body of a lambda may use free names
    within itself. these are the heart of the issue
    of returning lambda's as values. if the lambda
    uses no free names and only relys on it's bindings
    then a function pointer can act as a stand in for that
    procedure in local contexts, because the pointer encodes
    all of the information required to call the procedure.

    when a lambda uses a local name however, things can get tricky.
    because since the local name and the lambda are both values,
    we can imagine returning the lambda and not the value.

    this breaks the code, because we are talking about a C style
    procedure that local value is only alive for as long as the
    defining scope is alive. but the lambda could have it's 'lifetime'
    extended (not truly, but conceptually) by being returned.
    this means we need to extend the lifetimes of any free names
    that are used by the lambda somehow. my approach is going to
    involve adding each free name to the front of the argument list
    from the llvm perspective. but within our code we denote that these
    arguments are free arguments. which allows us to apply them to
    user calls of the lambda, which will now be providing less than
    the required number of arguments from the llvm perspective.
    but because the arguments are free in the local scope we can apply
    the underlying procedure by simply applying the free name arguments
    implicitly, when the user calls the procedure explicitly.

    this is the sameish strategy to closures in general, except that
    the closure itself holds the free variable as part of the already
    applied arguments to the procedure. and within the defining scope
    we apply the free arguments implicitly at each call site.
    note: if we handle the locally defined procedure with a closure, then
     the value of the free variable at definintion time is what is captured
     by the closure, and any subsequent updates to the local name would
     not be reflected within a call of the procedure, which would be confusing
     and frustrating as hell for programmers. however, when we consider this
     problem with respect to using closures to represent local definintions
     being returned there is no way of updating the local name after we
     return the lambda out of the defining scope. so using a copy of the
     value will always have the most final value that the local name held.

     in code this is
    {
      a := 0;
      b := 0;

      f0 := \x:Int, y:Int=> a * x + b * y; // the point of definition

      r0 := f0 1 1; // will be 0 * 1 + 0 * 1 ~> 0

      a <- 1;
      b <- 1;

      r1 := f0 1 1; // should be 1 * 1 + 1 * 1 ~> 2
                    // but if we capture the values
                    // only at time of definition
                    // it will still be 0 * 1 + 0 * 1 ~> 0

    }

    this is sort of the natural consequences of static scoping
    and strict evaluation. since we have to force evaluation until
    the term becomes something we can represent within a memory cell
    or it's an error, we are forced to build up formally a closure
    object to hold locally. because this is the way that functions are
    conceived of as local objects. they can't really be local objects.
    because in the end it's just a memory location we are jumping to
    after placing some values in locations agreed upon by both caller
    and callee. and returning data is the same, it's just a location that
    both the caller and callee know about. and can thus store/retrieve
    information from.

    since we have to store the value into a cell whose lifetime becomes
    distinct from the lifetime of defining scope, so we can then pass the
    function around as a value, we now have to update two locations to
    keep what is conceptually the same value up to date, and not have the
    program 'go wrong' as it were.

    so, in the basic implementation we simply capture and be on our
    way, but that only really works for functions that don't capture any
    free names. a side effect of this implementation is that
    the value the function captures is then not the same memory cell that
    the value is within it's defining scope,
    even though conceptually these two values are the 'same'.
    at least to me, the implicitly captured
    free usage of the name within the procedure, (essentially dynamic scoping
    of the lambda) would have the same value as whatever the names value is
    at any point within the procedure. so that calls to the lambda that happen
    after the definition of the lambda would still use the most up to
    date value of the local scope, even though that is no longer going
    to happen naturally as a consequence of the definition of the language.

    the only way that i can think to accomplish this is to, when we
    apply a closure local to it's definition, we somehow store some
    more metadata that adds a few extra instructions to a call of
    a locally defined lambda, that before application, load
    the most up to date values of the free names and store them
    into the closed over values, so that the application uses the
    most up to date value.
    (constants and anything that isn't updated doesn't need that
     treatment so we can simply not emit these instruction where
     we don't need them.)
    the other solution to this would be instead closing over a
    pointer to the stack location then we can always unwrap the
    most up to date value, but then we are right back in
    the first conundrum, the local data is not valid after the
    local scope exits.
    extra copies are read/written when we need the value,
    like when we have a volatile name in C.
    and like when we pass things by value, we emit extra loads
    and stores so we pass in the most up to date value from the memory cell.

    however, closed over values are still applied just like regular
    arguments, because in llvm we still have the same C style function
    type, just with the free names as now implicit first arguments.

    so, broadly, application has two choices, but they are
    distinct for both things. so, locally defined functions
    are going to have essentially dynamic scope with their
    defining scope. this is supported by implementing the
    lambda as having all free arguments as implicit first
    arguments to the procedure. we would like to simply
    capture the lambda into dynamic storage along with the
    free names (just like when we partially apply the function as well.)
    however, if we tie this emmiting to the definintion site only,
    any updates to the name we closed around will not be
    reflected within the value we closed over.

    this isn't a problem if the scope has already been run and
    returned, so there is no way to update that variable anymore.
    we don't need to worry about it.

    however if we consider the
    case where we are applying a local lambda within a loop,
    and the programmer happens to capture the index implicitly
    each run of the loop will be called with the same captured value
    with no change to the index, even if the local code updates
    the local index. this is a major problem. so, to remedy this,
    when we apply a locally defined procedure, like a lambda,
    if that lambda captures free variables, before we can apply
    the lambda we need to emit a sequence of instructions which
    loads the most up to date values of the free names and
    stores them into the dynamic storage of the closure,
    thus updating the value that was captured, and then
    application of the closure object can procedure as it was before.
    pull each closed over value and each applied value into a
    single call expression. (that is, the arrangement of a valid
    call term)

    (although we could also just apply the free variable directly from
     the load, and elide storing it in the dynamic allocation.)


     so, if we are talking about an array of pointers to
     members, and a function pointer in the first position,
     the only way we can call any possible given function
     without directly knowing the typesignature is to
     force every type signature to be identical from the
     perspective of llvm. namely
     void fn (void* sret, void* args);

     then we emit two type declarations per function, to
     be used strictly within the procedure itself, which
     casts these typeless pointers to pointers to it's
     return value allocation, and it's argument value allocation.

     like, we want to call a procedure from llvm we pass it a
     pointer to the allocation for the return value in the caller
     and the argument list in the caller. then we evaluate the call
     instruction.
     then within the procedure we perform two bitcasts at the
     top, namely casting the void* arguments to the
     return type and argument(s) type respectively.
     then we can emit GEPs stored into to local allocations
     named the correct things from the argument list within
     the body of the procedure. and when we want to get
     the return value we load it's local allocation which is
     a Ref T to the return allocation, where we can perform
     assignment. and because it's a return allocation the
     resulting scope knows about it.
  */

  // arg0.Type -> arg1.Type -> ... -> argn.Type -> body.Type()
  std::vector<llvm::Type*> formal_argument_types;

  for (const Argument& arg : this->formal_args)
  {
    formal_argument_types += arg->getType();
    // bind the argument's name to it's type,
    // so that terms composed of the arguments
    // name will have the correct type.
    lambda_env.GetSymbolTable().Bind(arg.GetName(), std::make_shared<TypeLiteral>(dummy_location, arg->getType()));
  }

  Judgement body_type_jdgmt = this->body->Getype(lambda_env);

  // unbind all of the arguments from the local
  // symbol table, so that codegen can bind the
  // names to new values.
  for (const Argument& arg : this->formal_args)
  {
    lambda_env.GetSymbolTable().Unbind(arg.GetName());
  }

  if (body_type_jdgmt.IsAnError())
    return body_type_jdgmt;

  std::shared_ptr<TypeLiteral> body_type_ptr = dynamic_cast<std::shared_ptr<TypeLiteral>>(body_type_jdgmt.GetTerm());
  llvm::Type* literal_body_type = body_type_ptr->GetLiteral();

  bool is_valid_return_type = llvm::FunctionType::isValidReturnType(literal_body_type);
  llvm::FunctionType* formal_type = nullptr;

    // okay, so we would love to be able to simply build a function type here.
    // but if the programmer stated a return type that is larger than
    // llvm can build a return instruction with, then we have to pass it
    // as a hidden first argument to the function, where the particular
    // arguments memory is caller allocated. to communicate this to the
    // optimizer we give the argument the sret attribute. or maybe this
    // makes llvm do it for us?
    // additionally, if we encounter
    // an actual argument whose type is to big to fit in a single register
    // we promote the arguments type to a pointer type, and
    // give that argument the byval attribute. i think this also let's
    // us pass pointers with the correct type directly, and llvm will
    // do the byvalue copy? yes, yes it does, the parameter will
    // formally have pointer type. but because of the byval attribute
    // llvm will do byval semantics
    // with this line of thinking, if the programmer explicitly
    // passes an argument by reference, then we can add the byref
    // attribute, which lets us pass the address of the local
    // memory directly.


  if (is_valid_return_type)
  {
    formal_type = llvm::FunctionType::get(literal_body_type, formal_argument_types, /* is-vararg? */ false);
  }
  else
  {
    // we cannot return the type through a single register, so we need to
    // return it via a sret parameter.
  }



  return Judgement(std::make_shared<TypeLiteral>(loc, formal_type));
}

Judgement Lambda::Codegen(const Environment& env)
{
  // register the lambda in the module, and attach it's body.
  // then return a reference to the function.
  Location dummy_location;


  // since we are generating code, we have already done typechecking,
  // thus the cached type will be filled.
  llvm::FunctionType* fn_type = llvm::cast<llvm::FunctionType*>(cached_type);

  // llvm::PointerType* fn_ptr_type = fn_type->getPointerTo(fn_type->getPointerAddressSpace())

  // generate a new name for our procedure,
  // this is a name of the form 'l' + '(0-9*)'
  // where what particular value is some number
  // related to the number of times we have called
  // Gensym.
  std::string anon_name = Gensym("l");

  // we know enough information to register the lambda to the module now.
  llvm::Function* fn = (env.GetModule()).getOrInsertFunction(anon_name, fn_type);

  // we have the function, so now we need to attach
  // a body. first we need to generate it
  /*
  Judgement body_jdgmt = body->Codegen(env);

  if (body_jdgmt.IsAnError())
    return body_jdgmt;
  */

  // since we want to support assignment,
  // we will need to emit stack allocations
  // for each argument.

  std::vector<Argument>::iterator formal_arg = formal_args.begin();

  for (llvm::Argument& arg : fn->args())
  {
    arg.setName(formal_arg->GetName());
    formal_arg++;
  }

  // we need a new environment, with the Lambda's
  // symboltable bound instead of the outer symboltable
  // we also need to either set/reset the IRBUilder's
  // insertion point, to accomplish this, we build a new
  // IRBUilder with the correct insertion point.
  std::shared_ptr<IRBuilder> lambda_builder = std::make_unique<IRBuilder>(env.GetContext());
  Environment lambda_env(env.GetInternedNames(),
                         env.GetInternedOperators(),
                         *this->symbols,
                         env.GetBinopTable(),
                         env.GetUnopTable(),
                         env.GetTargetTriple(),
                         env.GetDataLayout(),
                         env.GetContext(),
                         env.GetModule(),
                         *lambda_builder);

  const IRBuilder& bldr = lambda_env.GetIRBuilder();
  // build an entry block, and insert it into the function,
  // then set it as the IRBuilder's insertion point, so we
  // can build the argument allocations. llvm optimizations
  // work better if all a functions static allocations are
  // in it's entry block. (hence garunteed to run once.)
  llvm::BasicBlock* fn_entry_point = llvm::BasicBlock::Create(lambda_env.GetContext(), "entry", fn);

  // as an aside, there are a lot of different ways that llvm
  // want's to build the different structures. ususally with
  // memory it manages we are building objects by calling
  // static methods from other classes, which handle the
  // underlying memory management, this is true for most LLVM
  // IR objects. however, basic blocks and global variables are
  // built by calling their constructors explicitly, and then
  // that constructor takes the insertion point of the
  // module or the Function. my question is,
  // why not just use the same pattern as before and, when
  // you want a new basic block within the function you
  // just ask the function to build it for you. then it
  // lines up with the way that we build instructions, from the
  // IRBUilder. (although we also call the constructors directly
  // when we build the context and the module and the IRBuilder itself.
  // but again, there we are talking about standalone elements of
  // the program. a basic block cannot just freely float about the
  // program to be evaluated willy nilly, like the module can, or the
  // IRBUilder can.)
  bldr.SetInsertPoint(fn_entry_point);

  // - build an allocation point for the argument,
  // - initialize the stack allocation with the argument value
  // - bind the stack allocation into our symboltable.
  for (const llvm::Argument& arg : fn->args())
  {
    llvm::AllocaInst* var = bldr.CreateAlloca(arg.getType(), 0, nullptr, arg.getName());

    bldr.CreateStore(arg, var);

    (lambda_env.GetSymbolTable()).Bind(arg.getName(), std::make_shared<ValueLiteral>(dummy_location, var));
  }

  // now that our symbol table is set up to hold
  // our argument bindings, we can generate code
  // for the body. (which presumably uses those
  // memory locations.)
  Judgement body_jdgmt = this->body->Codegen(lambda_env);

  for (const llvm::Argument& arg : fn->args())
  {
    (lambda_env.GetSymbolTable()).Unbind(arg.getName());
  }

  if (body_jdgmt.IsAnError())
    return body_jdgmt;

  llvm::Value* result_value = (dynamic_cast<std::shared_ptr<ValueLiteral>>(body_jdgmt.term))->GetLiteral();

  // we only have types that are such a size that they
  // can be returned in a single register. so llvm supports
  // returning them through a return instruction, if the
  // type is larger than a single word, it must be returned
  // via a caller stack allocation, a pointer to this allocation
  // added as the first argument to the procedure, and
  // the by-value nature of return values consisting of a deep copy
  // being generated for the return value.
  bldr.CreateRetVal(result_value);

  std::string buf;
  llvm::raw_string_ostream fn_verification_errtxt(buf);

  bool good = llvm::verifyFunction(fn, fn_verification_errtxt);

  if (!good)
  {
    fn->eraseFromParent();
    // raise any llvm error into a Error
    return Judgement (Error(loc, fn_verification_errtxt));
  }

  // so, here is where we would usually return the Function*,
  // through a Value*. instead, because Function's cannot be
  // stored locally, we wrap the address to the function in
  // an anonymous structure and return that as the value.
  // this means that we can store this result locally.
  // of course, as a result, application expects an anon structure
  // with a function pointer in the leftmost position.
  // and uses the elements held in the rest of the structure
  // as the actual arguments to the function held within,
  // and uses the arguments provided within the actual_argument
  // term to fill in the rest of the arguments of the call.

  // build a function pointer
  llvm::FunctionType* fn_type = (llvm::cast<llvm::FunctionType*>(fn->getType()));
  llvm::Constant*     fn_addr = llvm::ConstantExpr::getPointerCast(fn, fn_type->getPointerTo());

  // store the pointer into a closure for local storage of
  // the function.
  std::shared_ptr<Tuple> lambda_closure = std::make_shared<Tuple>(loc, {std::make_shared<ValueLiteral>(dummy_location, fn_addr)});

  Judgement closure_jdgmt = lambda_closure->Codegen(env);

  // notice how instead of emmitting an allocation
  // within Boolean, or Integer, or Lambda we instead return
  // an object which represents that particular value,
  // which can then be used by the outer term to do
  // something with. for instance if the parent Ast
  // is a binop, we expect to be able to use the value
  // returned through right->Codegen(env) to construct a
  // binary expression. if the parent node is a bind expr
  // then we construct and emit an allocation.
  // so, this is a consistency with Object Codegen.
  // Objects return llvm::Values* to be used by further
  // calls to the IRBuilder; and Ast nodes do things with
  // Objects.
  return closure_jdgmt;
}

/*
  a formal procedure is like a lambda,
  except that we know what we want to
  name the procedure at time of creation,
  this means we know what name it is going
  to have within the modules symboltable.
  however, within our own symboltable,
  i think we are still going to store a
  function pointer plus nil, to make
  application more streamlined.
  (it then only has to handle the case
  where we have given it a pointer to a struct
  with a fn ptr in first position, and
  the actual arguments bound in each
  subsequent position.)
*/


}





// ----
