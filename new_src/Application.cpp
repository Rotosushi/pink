
/*
  If the actual argument list length is one,
  and that one element is itself a tuple,
  (not a named struct) then
  we can convert that tuple into the
  argument list.
*/
#include <string>
#include <memory>
#include <vector>

#include "llvm/Support/raw_ostream.h"

#include "llvm/ADT/APInt.h" // for ArrayRef<T>

#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"


#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"
#include "Application.hpp"

namespace pink {

Application::Application(const Location& loc, std::shared_ptr<Ast> fn, std::vector<std::shared_ptr<Ast>> args)
  : Ast(Ast::Kind::Application, loc), function(fn), actual_arguments(args)
{

}

bool Application::classof(const Ast* ast)
{
  return ast->GetKind() == Ast::Kind::Application;
}

// in effect, each Clone() needs to make
// copies of each of it's subelements.
// since there is a subelements of language
// within each position of the vector, we
// need to clone each of them, and the function
// descriptor that the application term
// was constructed with.
std::shared_ptr<Application> Application::Clone()
{
  std::vector<std::shared_ptr<Ast>> clone_arguments;

  for (std::shared_ptr<Ast>& argument : actual_arguments)
  {
    clone_arguments += argument->Clone();
  }

  return std::make_shared<Application>(loc, fn->Clone(), clone_arguments);
}

std::string Application::ToString()
{
  std::string result;
  result += function->ToString() + " ";
  int i = 0;
  for (std::shared_ptr<Ast>& argument : actual_arguments)
  {
    result += argument->ToString();

    // print a space between each argument.
    if (i < (actual_arguments.size() - 1))
    {
      result += " ";
    }
  }
  return result;
}

/*
  the type of an application term is either
  the result type of the function object,
  - if the actual argument list size matches
  the formal argument list size, and each
  argument has the correct type.
  - or, if the actual argument list size is
  smaller than the formal argument list size,
  the result type is the type of the rest
  of the application; if each provided actual
  argument matches in type with the formal argument
  at that position, up to the point where we have
  more formal arguments than actual arguments,
  then we can construct a new function type, whose
  argument list is the rest of the formal arguments
  starting from the point at which there was no more
  actual arguments for this term, and whose return type
  is the return type of the function.
  - or, if the actual argument list size is greater
  than the formal argument list size, and we consider
  that an error.
*/
Judgement Application::GetypeV(const Environment& env)
{
  // in the end we walk the actual argument list
  // twice, and the formal, once.
  Judgement fn_jdgmt = function->Getype(env);

  if (fn_jdgmt.IsAnError())
    return fn_jdgmt;

  // unless something is seriously wrong, this dynamic_cast should never fail
  // we just checked it's integrity with the boolean check above.
  // (at least, we should have, hence the 'seriously wrong')
  llvm::Type* fn_jdgmt_type = (dynamic_cast<std::shared_ptr<TypeLiteral>>(fn_jdgmt.term))->GetLiteral();

  if (!fn_jdgmt_type)
    FatalError("unexpected unitialized TypeLiteral!", __FILE__, __LINE__);


  // however, if we see a valid type, and it's simply
  // not a function type, that's the programmers fault.
  llvm::FunctionType* fn_type = dynamic_cast<llvm::FunctionType*>(fn_jdgmt_type);

  if (!fn_type)
  {
    std::string errtxt, buffer;
    llvm::raw_string_ostream left_type_string(buffer);
    fn_jdgmt_type->print(left_type_string);

    errtxt += "application requires a callable in the leftmost position. [";
    errtxt += left_type_string.str() + "] is not callable.";
    return Judgement(Error(errtxt, loc));
  }

  std::vector<llvm::Type*> actual_argument_types;

  for (std::shared_ptr<Ast>& argument : actual_arguments)
  {
    Judgement arg_type_jdgmt = argument->Getype(env);

    if (arg_type_jdgmt.IsAnError())
      return arg_type_jdgmt;
    else
    {
      llvm::Type* actual_argument_type = (dynamic_cast<std::shared_ptr<TypeLiteral>>(arg_type_jdgmt.term))->GetLiteral());

      if (!actual_argument_type)
        FatalError("unexpected uninitialized TypeLiteral!", __FILE__, __LINE__);
      else
        actual_argument_types += actual_argument_type;
    }
  }

  std::size_t num_formal_arguments = fn_type->getNumParams();
  if (actual_argument_types.size() > num_formal_arguments)
  {
    std::string errtxt;
    errtxt += "more arguments provided [" + std::to_string(actual_argument_types.size());
    errtxt += "] than formal arguments accepted [" + std::to_string(num_formal_arguments);
    errtxt += "]";
    return Judgement(Error(errtxt, loc));
  }

  llvm::FunctionType::param_iterator fi = fn_type->param_begin();

  for (std::vector<llvm::Type*>::iterator ai = actual_argument_types.begin(); ai != actual_argument_types.end(); ai++, fi++)
  {
    if ((*ai) != (*fi))
    {
      // llvm::Type's and Value's only support
      // dumping themselves as strings if they
      // are also putting that string into a stream.
      // luckily they also provide an interface for
      // building a stream out of a local string.
      std::string formal_argument_string;
      llvm::raw_string_ostream handle(formal_argument_string);
      formal_argument_types[i]->print(handle);

      std::string errtxt;
      errtxt += "actual argument [" + std::to_string(i);
      errtxt += "] type [" + actual_arguments[i]->ToString();
      errtxt += "] does not match formal argument [" + std::to_string(i);
      errtxt += "] type [" + handle.str();
      errtxt += "]";
      return Judgement(Error(errtxt, loc));
    }
  }

  // so, we know that actual arguments size is not
  // greater than formal arguments size, but now
  // we need to know, is it less than or equal to?
  // how do we know that? well considering we were
  // just bumping an iterator along a container of
  // formal arguments...
  if (fi == fn_type->param_end())
  {
    return Judgement(std::make_shared<TypeLiteral>(loc, fn_type->getResultType()));
  }
  else
  {
    // make a new function type.
    // from the rest of the formal type
    std::vector<llvm::Type*> closure_argument_types;
    while (fi != fn_type.param_end())
    {
      closure_type += *fi;
      fi++;
    }

    llvm::FunctionType* closure_type = llvm::FunctionType::get(fn_type->getResultType(), closure_argument_types, /* is_vararg? */ false);

    return Judgement(std::make_shared<TypeLiteral>(loc, closure_type));
  }
}


/*
  returns a value representing the result value of this application.

  so the general outline of this procedure is,
  -) compute the lhs, which is either a lambda
     or a bound procedure. we are given either
     a closure object directly, or an opaque ptr
     to a closure object.

     we cannot stack allocate closures
     the only way we can
     support 'holding' a closure in a stack is if
     it's size is constant, but if we are in the case
     where the input argument is modified and then returned,
     the return type depends upon the precise type of the
     input argument, but we had to make the type of a
     closure opaque specifically because we couldn't make
     any assumptions about the size of the closure from the
     callee side. so, because the size of the object itself
     is dependant upon the runtime path of the code itself.
     we cannot store it statically. this is the same reasoning
     behind c++ vectors, aka dynamically allocated arrays.

     so, they need to be both opaque, and dynamically allocated.
     this is not good. this means that we have to rely on the
     dynamic memory allocator to build applications. as
     since the precise argument list needs to be generated
     at runtime, the only place in memory to do that is the
     dynamic allocator. the only cases where we don't have
     to dynamically allocate are those case where the usage is
     exactly replaceable with a bare function pointer argument.

     well, if we are given a pointer to some allocated memory
     which holds the closure, we want to build a call expression
     passing in the function pointer, each of the closed over
     arguments, and each of the actual arguments.
     but again, normally each call
     of a function pointer in some specific location corresponds to a
     constant number of arguments we have to apply to the
     underlying procedure. (N = 0, in every case where we are
     applying the function pointer) however
     when we are talking about a closure object, we can have
     N many already bound arguments held within the structure.
     when the underlying procedure requires M many actual arguments
     to provide. this changes the underlying functions type signature
     in that it binds a formal argument to an actual value, and we can think of
     the closure as a 'new' function which has the same type as the
     formal procedure, except it no longer expects the argument that
     was just bound, hence it has the type of the rest of the function.

     the problem is that the number of held arguments are runtime
     dependant, which means that the call term we need to build
     is runtime dependant as well right? well, a llvm 'call' term
     is really just a jump instruction paired with a calling convention.
     the function pointer is really just the address we are jumping to.
     so if the procedure has been written to expect M arguments. and the
     closure provides N bound arguments, and the application term
     provides another X actual arguments. if N + X == M
     the type of the rest of the application becomes the return type
     of the underlying procedure.
     if N + X < M then the result is a closure.
     if N + X > M then the result is an error.
     if we consider applications of closure objects,

    1 -) X is constant for any given application term, because the
     programmer had to type in some knowable finite list of
     names to describe said actual argument list. this means
     that the application of a closure will always provide
     the same number of new actual arguments each time
     the application is run.
    2 -) M is constant for any given function pointer, because each
     function is unique for any given program.
     (even if two functions have exactly the same type signature
      they can have different behaviors, think + and -)

    note that M is not the number of parameters of the functions
    type signature within the argument or return parameter, unless the closure
    object itself binds no arguments. but there is exactly the rub.

    1, 2 taken together means that for any given codepath the
    application of a passed closure is either going to produce the
    result value, or another closure around the same procedure.
    and that is dependent upon Y which is the number of arguments
    in the parameters type signature. (Y is also the difference between
    the number of arguments the function expects M, and the number of
    arguments the closure has bound N)
    notice however, that Y is constant at compile time and not runtime dependant.
    because it is related to the signature of the callable object that is
    being passed, which always specifies it's return value, because it is
    a parameter type.
    never at runtime will this change. the procedure's argument will
    always have the same type, and only closures whose presented
    function type, (the type of the 'rest' of their application.) match the
    formal arguments type can be passed at runtime. and given that the
    closure's type can be expressed by us, a typechecking phase can
    record the presented type of the closure and ensure that it is being
    applied. because we always know what the rest of the type of the
    closure is during typechecking. for any given codepath
    written to apply the passed procedure more than once
    where at one time we do provide enough arguments and
    at one time we do not. well, the only way that more
    than one application term can be introduced is either in
    a sequence, or in a conditional expression. conditionals
    are ruled out immediately as soon as we think about the
    fact that both branches have to have the same resulting type.
    and sequence isn't much of a problem because the value is either
    bound locally or ignored, both of which are valid
    and known behaviors.
    N is runtime dependant, because the same procedure argument
    with the same formal type, could have any number of prebound objects before it gets
    to this call point, which is going to provide X more arguments.

     if we consider the empty closure around a lambda. to represent
     that lambda within the local data of the procedure.
     this closure holds no bound arguments, this means that when
     we go to build a call term of such a closure, the actual argument
     list length must match the formal argument list length for us to
     build a call term. otherwise, we provide fewer arguments than the
     lambda expects and build a new closure as the result.
     and either would be reflected in the types of the value being
     computed over. (you either physically typed in enough arguments
     to apply, or you didn't.)

     if we recall that an arguments prototype describes precisely the types
     of procedures that this function can even apply. and that
     prototypes will be static at compile time. this means that
     for any function we recieve as an argument, we need to give each
     the same number M of actual arguments to apply the underlying
     procedure, and if we are building an application or another closure
     that itself will be reflected in the type signature of the procedure
     applying the closure. this means that the code which emits
     the call can rely on the number M, that is the number of actual arguments
     given resulting in a call or another closure, and that number M
     is constant for the lifetime of the procedure. because if the
     procedure contains applications, those applications are finite
     knowable lists.
     however, the existance of a Runtime dependant number N,
     means that if we want to build a stack allocation to hold each of
     the argument values, then we would need to calculate
     the size of that allocation at runtime. however, this alloca
     would be a valid location to hold that data for the application of
     the underlying procedure. because it would not be deallocated
     until after the call returned.

     so if we built a pass which constructs Call from a variable
     sized list of arguments. to a procedure which is recievied
     from a lookup table. recieving enough arguments via an
     actual argument list, to push enough arguments onto the
     stack to the correctly jump into the procedure.
     then if we were on the codepath
     which resulted in a call term, we would build a section of
     code which grabbed the bound arguments and the actual arguments
     and placed them onto the stack in the correct expected order
     before jumping to the provided function.
     and if we were on the codepath which returned another closure
     then we would build a new closure on the heap holding
     the bound argument values and the actual argument values and
     the same underlying function pointer as the provided closure.
     (because we are emulating binding another actual value to
     that same procedure. via a new immutable closure.)

     --- okay, if the closure is dynamically allocated, and we get a typed
         function pointer via a lookup table, then really, we can store
         GEP pointers stored as integers in the type descriptor array.
         then we can pass these pointers into the procedure as arguments,
         for byval parameters that will cause llvm to emit a copy.
         if we captured a reference, then it must be a reference to heap
         memory.

      (aside: passing a pointer to local memory into a closure,
       we cannot allow that closure to be returned from the procedure.
       we cannot allow pointers to the stack to be returned from procedures.
       we can only allow pointers to the stack to be passed into procedures)
       (similarly we wouldn't allow a programmer to capture a pointer to
       the local stack in a tuple, and then return it.)
       depending on how we detect this and how we write it eventually, we
       will either want to disallow assignment of a stack pointer to local
       memory, or we could use metadata to detect if we are returning a
       stack pointer directly, or if the structure we are returning captured
       a local stack pointer.
       needing to store a collection of pointers to functions implies
       that we need to unify the function signature we are working over.
       we cannot store different function pointer by the semantics of
       either c/c++ or llvm. so the signature will be

       void fn (u8* sret/returned, u8* byval)

       or maybe

       void fn (u8* sret/returned, size_t ret_sz, u8* byval, size_t arg_sz)

       so, then calls to the proc will coincide with the  declaration
       of a type to cast to which describes the layout of the arguments
       and allows the caller to set up the structure, and the callee to
       pull members (arguments) out of it. i'm not sure if the size is
       needed if we have the type during compiletime. but the thing is
       we are specifically dealing with the fact that a specific part of
       application needs to be done at runtime. namely the construction
       of the argument list and the return value itself.
       since we are talking about statically typed calls, (no overloading)
       we can rely on the fact that the return value will be static, and
       deducable during typechecking. (and i think we will only allow return
       types to be different accross a single type heirarchy. (like how llvm
       returns Value* from the IRBuilder, but some are specialized to
       the specific instruction representing the value, like LoadInst
       StoreInst and AllocaInst. but i get ahead of myself))
       this means that once we see what static call this function we are
       implementing is applying, we will be able to see the return type.
       we can allocate enough space at the call site with that information.
       what changes is the particular number of arguments we are applying to
       the function which looks like the binding we accepted as argument.
       (the discussion should be the same, but regarding the presented function
        type of the closure itself when considering a local decl or returned closure
        from an application term.)
       so, we should be able to deduce the number and type of arguments from
       the function pointer we are returned. (given that the functions need to
       have the same signature to be stored in an array. we unified their
       types.
       (we could use a giant anon structure i think, or a bunch of local
       caches.))
       if we simply consider the aggregate sizes of the
       actual argument list, and the closed over argument list.
       we could simply pass in a memcpy of both into the same
       allocation. on the inside we can cast to the known
       allocation of our aggregate. and pull out element pointers.




       then, we build an anon struct of values (which can in the
       future include pointer values, (a.k.a. reference parameters))
       which comprises the function list. and pass that as the
       arguments to the function. each function can be emittited such
       that it associates the name in the table with the GEP of the
       argument.

       if the return type fits in a single register then the functions
       first argument gets the 'returned' attribute, otherwise
       'sret' which have the same semantics of making the argument
       act like a return value, (meaning retuning the value from
       the procedure means assigning to this structure), except
       for either register sized values, or larger values.wwwwwwwwwdwwwwwwwwww

     this means that application always needs to handle dispatching
     to procedures via the runtime.
     can we even extract a typed function pointer out of this opaque structure?
     i think we would need to use a lookup procedure to get a typed ptr.

     we can store the pointer at some known address. but we cannot
     construct a typed call to the procedure.
     we must blindly
     place the already applied arguments into their correct
     memory locations from the actual caller location.
     but the thing is, we bound those arguments to
     the closure by using another application term.
     so we did the typechecking already.

  -) compute the arguments, which gives us references
     to their values.
  -) check if we have enough arguments to apply.
  we have enough arguments:
     -) build a call term from the arguments held within
        the closure (zero or more), and the actual arguments,
        (one or more).
  we have not enough arguments:
      -) build a new closure, from the arguments held within
         the old closure (zero or more) and the actual arguments.
         (one or more).

  return the resulting value.

  okay, as an aside, i think the general strategy
  for codegen is,
    -) compute the value(s) which the term is
        grammatically talking about.
    -) allocate space for the value to live during
        the runtime of the program.
    -) return a ptr to the result.
  this then means that we do not need ValueLiterals,
  we need AllocaInstLiterals.
  and from this we can make the additional
  observation that running a mem2reg or sroa pass
  and an InstCombiner pass would result in much cleaner
  code. as it would fold some of the superfluous
  memory allocations that this codegen strategy is
  going to insert.
*/
Judgement Application::Codegen(const Environment& env)
{

  Judgement fn_jdgmt = function->Codegen(env);

  if (fn_jdgmt.IsAnError())
    return fn_jdgmt;


  llvm::Value* fn_value = (dynamic_cast<std::shared_ptr<ValueLiteral>>(fn_jdgmt.term))->GetLiteral();

  llvm::ConstantStruct* closure_literal = llvm::cast<llvm::ConstantStruct*>(fn_value);

  if (!closure_literal)
  {
    // we did typechecking, which should have caught
    // that we weren't receiving a callable object
    // so this is a FatalError, and i assume it only
    // gets triggered if i have made a mistake somewhere else.
    std::string buf;
    llvm::raw_string_ostream hdl(buf);
    fn_value->print(hdl);

    FatalError("Function llvm::Value [" + std::string(hdl.str() + "] is not a ConstantStruct"), __FILE__, __LINE__);
  }


  // first we codegen the actual arguments.
  std::vector<llvm::Value*> actual_arg_values;
  for (const std::shared_ptr<Ast>& arg : actual_arguments)
  {
    Judgement actual_arg_jdgmt = arg->Codegen(env);

    if (actual_arg_jdgmt.IsAnError())
      return actual_arg_jdgmt;

    llvm::Value* actual_value = (dynamic_cast<ValueLiteral>(actual_arg_jdgmt.term))->GetLiteral();
    actual_arg_values += actual_value;
  }

  llvm::StructType*         closure_type = closure_literal->getType();
  llvm::StructType::element_iterator cti = closure_type->element_begin(); // (c)losure (t)ype (i)terator
  llvm::Type*         first_element_type = *cti;

  cti++; // bump to the start of the formal arguments.

  if (!first_element_type->isPointerType())
  {
    std::string buf;
    llvm::raw_string_ostream hdl(buf);
    first_element_type->print(hdl);

    FatalError("First element of Tuple [" + std::string(hdl.str() + "] is not a Pointer"), __FILE__, __LINE__);
  }

  llvm::PointerType*  ptr_type = llvm::cast<llvm::PointerType*>(first_element_type);
  llvm::Type*         pte_type = ptr_type->getElementType(); // (p)oin(te)e_type

  if (!pte_type->isFunctionType())
  {
    std::string buf;
    llvm::raw_string_ostream hdl(buf);
    pte_type->print(hdl);

    FatalError("First element of Tuple, the ptr [" + std::string(hdl.str() + "] doesn't point to a Function"), __FILE__, __LINE__);
  }

  std::vector<std::shared_ptr<Ast>> app_args;
  const IRBuilder& bldr = env.GetIRBuilder();

  llvm::FunctionType*             fn_type = llvm::cast<llvm::FunctionType*>(pte_type);
  llvm::FunctionType::param_iterator  fti = fn_type->param_begin();    // (f)ormal (t)ype (i)terator
  std::vector<llvm::Value*>::iterator aai = actual_arg_values.begin(); // (a)ctual (a)rgument (i)terator
  // we have the function param type iterator,
  // we have the closure argument type iterator,
  // we have the actual argument values iterator.
  std::size_t actual_arg_length  = actual_arg_values.size();
  std::size_t closure_arg_length = closure_type->elements().size();
  std::size_t formal_arg_length  = fn_type->params().size();
  std::size_t applied_arg_length = (actual_arg_length + closure_arg_length);

  if (formal_arg_length > applied_arg_length)
  {
    // build a new ConstantStruct holding the function pointer
    // and all of the applied arguments and return that.
    std::size_t elemidx = 0;

    // (G)et (E)lement (P)ointer is an instruction which
    // indexes it's second operand with the value held within
    // it's third operand. the type being indexed is described
    // by the first operand. so, in this call, we will be
    // getting a pointer to the first member of the ConstantStruct,
    // which is the function pointer, we then load it's value.
    // this 'unwraps' the pointer.
    llvm::Value* fn_ptr_ptr = bldr.CreateStructGEP(closure_type, closure_literal, elemidx++);
    llvm::LoadInst* fn_ptr  = bldr.CreateLoad(ptr_type, fn_ptr_ptr);

    app_args += std::make_shared<ValueLiteral>(dummy_location, fn_ptr);

    // load each other element.
    while (cti != closure_type.element_end())
    {
      llvm::Value* elem_ptr = bldr.CreateStructGEP(closure_type, closure_literal, elemidx++);
      llvm::LoadInst* elem  = bldr.CreateLoad(*cti, elem_ptr);

      app_args += std::make_shared<ValueLiteral>(dummy_location, elem);
      cti++;
    }

    while (aai != actual_arg_values.end())
    {
      // aai is an iterator over llvm::Value*,
      // which can directly be given to a tuple.
      app_args += std::make_shared<ValueLiteral>(dummy_location, *aai);
      aai+;
    }

    // we have each of the values representing all of the literals
    // that we want to build the new tuple out of held within
    // app_args, so build the tuple.
    std::shared_ptr<Tuple> new_closure = std::make_shared<Tuple>(loc, app_args);

    // the last thing we need to do in this function is build
    // the tuple comprising the new closure, this is handled by
    // the Tuple::Codegen method.
    Judgement new_closure_jdgmt = new_closure->Codegen(env);
    return new_closure_jdgmt;
  }
  else if (formal_arg_length == applied_arg_length)
  {
    // the tuple is composed of
    // the function pointer, plus the current captured argument list.
    std::size_t elemidx = 0;
    // build a call instruction using all of
    // the applied arguments and return the result
    llvm::Value* fn_ptr_ptr = bldr.CreateStructGEP(closure_type, closure_literal, elemidx++);
    llvm::LoadInst* fn_ptr  = bldr.CreateLoad(ptr_type, fn_ptr_ptr);
    llvm::Function* fn      = llvm::cast<llvm::Function*>(fn_ptr);
    llvm::FunctionCallee fncallee(fn);

    std::vector<llvm::Value*> call_args;

    while (cti != closure_type.element_end())
    {
      llvm::Value* elem_ptr = bldr.CreateStructGEP(closure_type, closure_literal, elemidx++);
      llvm::LoadInst* elem  = bldr.CreateLoad(*cti, elem_ptr);

      call_args += llvm::cast<llvm::Value*>(elem);
      cti++;
    }

    while (aai != actual_arg_values.end())
    {
      // we emitted the code for the actual arguments
      // above.
      call_args += *aai;
      aai++;
    }

    // we can't quite use this going forward, if the product of
    // a procedure is itself a tuple. which is a
    // core feature,  we need to modify the function's implementation.
    // however, since we don't have a sequence operator,
    // a functions body is either another function,
    // making it curried, thus we are not returning a
    // closure literal.  or the body is some other expression,
    // which is either a bindop or a unop. (or a bind, but
    // no sequence, no way to use a bound local.)
    // both of which operate on intrinsic types, and thus
    // their result types would be suitable to be passed
    // through a regular llvm::Call. which returns it's
    // value through a register usually, making it's size
    // restricted to one word.
    llvm::CallInst* fn_result = bldr.CreateCall(fncallee, call_args);
    return Judgement (std::make_shared<ValueLiteral>(loc, fn_result));
  }
  else
  {
    // TODO: change this to only display the arguments the programmer typed,
    //       because this message will reveal extra hidden arguments inserted
    //       to make the procedure work. also, this should have been caught,
    //       and thus is a FatalError
    std::string errtxt = "More Arguments given [" + std::string(std::to_string(applied_arg_length) + "] than the Function binds [") + std::string(std::to_string(formal_arg_length) + "]."));
    FatalError(errtxt, __FILE__, __LINE__);
  }
}


}



// --
