/*
  "A good programming language is to be as eggs."
  -- somebody other than me said this,
     but for the life of me i can't remember who,
     it is a fantastic quote though, and i agree.
     TODO: attribute this correctly.
     I translate the metaphorical meaning as,
     versatile, easy to use, clear in use,
     yummy, a strong binding ingredient, and
     simple. because that is what cooking with eggs
     is like, you can scramble some eggs, sure,
     and they can even be delicious, but there is
     also so much more! you could fry them, and then you even get
     subchoices over-easy/medium/hard, sunny side up, or basted,
     also you can make quiches, scrambles,
     and omlettes with all manner of flavor profiles!
     garlic, mushrooms, onions, and tomatoes are a particular
     favorite of mine.
     you can even use eggs to bake!
     they do wonders in bread, they pull double duty in
     cakes and brownies and pastry dough, and all manner of crusts,
     and what is a bun in the oven without an egg-wash
     to give that golden brown coat? all said and done
     eggs are a fantastic, central ingredient to cooking,
     and we should be thanking chickens every morning
     for the yummy food they provide. :D


  "There is no such thing as a hard problem,
  There are only problems which have yet to
  be decomposed into a constituent set of easy
  problems."


  "Failure deserves just as much respect as Success,
  if you fail, you are given an opportunity to learn
  something, respect that.
  Failure is just another path we take."



  "Isomorphism between numbers and (things and things)
   is what math captures. one pig, two pigs, three pigs!"
   (all are pigs, and there are N many of them)
   where N = 3;
   -- this is just neat, and i like the observation,
      so i'm preserving it.


   And so here are some words in a particular
   combination which you peice together into
   meaning something.
   I find it funny that
   this particular sequence amounts to nothing,
   except to call your attention to the fact that
   this is what is occuring.


   I am simply the human who is shuffling the
   characters around in these text files in
   accordance with some meaning, the meaning
   itself i sometimes need help in creating
   or getting correct, and i rely on other
   people to help me, since we have the internet
   i can find other peoples contributions
   without their asking, because they were
   kind enough to share. without this sharing,
   much learning would be behind a paywall,
   and thus accessible only to those with,
   and those without are truly then, those
   without. well, i want a new world where we
   don't make 'those without'. so to me, the
   sharing is a part of the essence of information
   itself. either it is freely shared information
   or I see it with doubt, and agenda. of course
   this doesn't mean that just because it is
   freely shared it is beyond doubt, or without
   agenda, what we can do, is approach it with
   our understanding of what freely shared means,
   and what things we can be sure of, and what things
   we cannot, and we can more accurately judge the
   truth of what is being said.
   what i can do though, is reflect on my
   understanding of freely sharing, and the difficulties
   i've had with creating and communicating information,
   and i can come to a fuller understanding of the
   truth of what is being communicated to me,
   what i am understanding myself, and what i want to
   and how i communicate with others.



  "Do not call up that which you cannot cast out."

  i.e. if you are providing a type, and it's meant
  to be used within a context, maybe provide a dandy
  basic interface to do so.

  basically, why isn't there a <getopt> standard header
  available to be linked into the willing program?

  why's it gotta be tied to my OS. when command line
  arguments are literally within the languages design!
  right there in argument list of main!

  they got this right with most other things though,
  pointers are made with

  &(...)

  and unmade with

  *(...)


  dynamic memory is made with

  malloc(...)

  and unmade with

  free(...)



a Lambda is a bind, plus an abstraction.

a bind is just the binding act.

a function definition is two binds plus the
abstraction part. (and we can allow a bind to
bind as many names as it wants.)

so what is the abstraction part of the lambda
then?

well, so we know the instruction sequence is enacting
the abstraction part. (x + x where x = 2,
this is translated into an assembly instruction
which enacts the required operation, and the
result is then available for use. in the same
way we imagine the term (x + x) stated within
a context that the name x has been bound to some
value, has the meaning of performing the operation
of '+' to the sequence of bytes which reperesent
the value that x reffers to, and then the result
is placed by the computer into the specified location
of the instruction.)

 so terms of the language themselves are sort of the,
 just the abstraction part?

 to the bind's just the binding part.

 that's what i'm seeing right now at least.

 x := 2

 is just a bind,

 x + x

 is just evaluating a no argument abstraction.

 \x : Int => x + x

 is the specification of a function, encapsulating
 the evaluation of an abstraction (x + x), over a new
 bound term, named 'x' with type Int.

 f := \x:Int => x + x

 is the bind of a name to a value.
 the value is a representation of
 the function specified by the lambda term.

 fn f (x:Int) { x + x }

 is the definition of a function 'f'
 which takes one argument 'x' of type int
 this function abstracts over the body
 specified between the "{ ... }" symbols.

  f 1

 is an application of the function 'f'
 passing in a value of type Int.

 (\x:Int=>x+x) 1

 is the specification of a function,
 and the application of a function,
 during compiletime we compute the
 definition, during the runtime we
 consider the lhs of the application
 term to be a function pointer plus
 an empty tuple of actual arguments.
 which we then apply an argument list
 of length one. since one is equal to one,
 this results in a c-style application of
 the c style definition. however,
 should the actual argument list be smaller
 than the formal argument list,
 instead of considering this an error,
 we instead capture this application of the
 procedure.
 which, given that the body of
 a statically typed procedure is the same
 for any given value of the type the procedure
 is defined for we can consider the address of
 the procedure plus knowledge of the type,
 to be equivalent to the procedure itself,
 in the same way we can consider a string of
 characters equivalent to it's address in
 a StringInterner. (we have an address, of the
 first character, plus knowledge of the type,
 luckily only ever a string. namely every interned
 string has the exact same type.) functions
 have unique types. which dictate what values can
 be passed in. so, what is unique about any particular
 application of a procedure, in the strictly evaluated
 sense, is which particular values are being placed in
 those arguments at that particular time.
 these two things together mean that we can consider
 a capture of a particular application equivalent to
 a capture of the functions pointer, plus the capture
 of the values which were provided by this call site.

 a closure can be considered as a subset of this
 action, if and only if we can also accept that

 fn outerf (x:Int, y:Int)
 {
    \z:Nil => x + y
 }

 resultfn := outerf 3 4

 result := resultfn ()

 result ~> 7

 within the compiler, given the above definition,
 we treat the inner  lambda, which the programmer
 said, took one argument, is actually a definition
 of this procedure, which takes three arguments.

 fn outerf (x:Int, y:Int)
 {
   innerfn := \x:Int, y:Int, z:Nil => x + y;
   return innerfn x y
 }

 resultfn := outerf 3 4

 resultfn ~> (innerfn, 3, 4)

 result := resultfn ()

 resultfn has a function type itself, which is
 distinct from, but also entirely derived from
 the function type of the underlying function.
  (innerfn) above is the pointer directly.

 namely, it has the type of the rest of the application.


we can break the lambda down into two things,
the binding of a name to a temporary value.

and the jumping into a sequence of instructions
which utilize this new names value.

so running the code is the abstraction act
in and of itself?
*/


#include <string>
#include <memory>

#include "Environment.hpp"


#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/APFloat.h"

#include "llvm/Support/CodeGen.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"



int main(int argc, char** argv)
{
  InitLLVM X(argc, argv); // declaration.
  /*
    the llvm context holds a lot of core datastructures
    of llvm, like the type and constant interners,
    and should be used in a per-thread basis, as
    there are no internal locks for multiple thread
    access of the same context.

    if we wanted to do multiple threads of compilation, say
    to compile a large program in parallel, due to the
    innate parallelism in the fact that ~different~ files
    are compiled, and then linked together, each file can be compiled
    separately, and in this situation, llvm needs a unique context per thread.
  */
  std::shared_ptr<llvm::LLVMContext> pink_context = std::make_shared<llvm::LLVMContext>();

  /*
    the IRBuilder is the exclusive way of constructing LLVM IR
    within the llvm api. we set up the builder relative to
    our context, this implies the same threadsafety discussion
    as above.
  */
  std::shared_ptr<llvm::IRBuilder>   pink_builder = std::make_shared<llvm::IRBuilder<>>(*pink_context);

  /*
   these functions are defined in
   llvm/Support/TargetSelect.h
   they initialize the TargetRegistry
   (i assume a member of the global LLVMContext)
   this initializes all of the
  */
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllDisassemblers();

  // get the target triple needed to load code into
  // the currently running process.
  // to define a more robust compiler,
  // this statement will need to be replaced with
  // one that dynamically finds the TargetTriple for
  // the installation/target machine, or allows the user to specify
  // searches through the TargetRegistry in the arguments to the program.
  llvm::StringRef pink_target_triple = llvm::sys::getProcessTriple();

  // to properly get the datalayout string
  // we are going to get LLVM to generate it
  // this means constructing a TargetMachine
  // based on the TargetTriple, plus a CPU
  // extra CPU options, Floating Point implementation.
  // a CodeRelocation Model
  std::string err;
  llvm::Target* pink_target = llvm::TargetRegistry::lookupTarget(pink_target_triple, err);

  if (!pink_target)
  {
    FatalError(err, __FILE__, __LINE__);
  }

  // these are more options that would need to be handled
  // in a platform independent way, should we want to emit
  // code for more than one platform.
  std::string pink_cpu = "x86-64"; // llc --help
  std::string pink_cpu_features = ""; // llc -march=x86 -mattr=help
  llvm::TargetOptions pink_target_options; // llvm.org/doxygen/TargetOptions_8h_source.html
  llvm::Reloc::Model pink_code_relocation_model = llvm::Reloc::Model::PIC_; // llvm.org/doxyge/CodeGen_8h_source.html
  llvm::CodeModel::Model pink_code_model = llvm::CodeModel::Model::Small; // idk what this option really means,
                                                                          // (literally: (P)osition (I)ndependant (C)ode.)
                                                                          // but, semantically, something to do with
                                                                          // always using absolute offsets/addressing
                                                                          // of references vs always computing
                                                                          // the addresses/offsets?
  llvm::CodeGenOpt::Level pink_optimization_level = llvm::CodeGenOpt::Level::None; // optimization level, like -O0 -O1 -O2 -O3

  llvm::TargetMachine* pink_target_machine = pink_target->createTargetMachine(pink_target_triple,
                                                                              pink_cpu,
                                                                              pink_cpu_features,
                                                                              pink_target_options,
                                                                              pink_code_relocation_model,
                                                                              pink_optimization_level);


  /*
    the llvm module is the core datastructure representing a
    module of compilation. this is what the header files in
    C/C++ specify, and this is what many modern languages
    make explicit in the text of the language. Files are
    modules in Python, and Modules have an Explicit Syntax
    in Haskell.

    Pink is probably going to follow Python here,
    and elide explicit module specification syntax,
    instead making the File itself, implicitly the Module.
    import/export syntax is going to be explicit, and
    any name not explicitly exported will be implicitly private.

    functions and global variables can be added to modules,
    and exported to other modules by setting the linkage
    attributes on the specific symbols. when considering
    using the JIT, or emmiting Object code, or Assembly.
    we use modules as the main 'currency' between the
    passes.
  */
  std::shared_ptr<llvm::Module>  pink_module  = std::make_shared<llvm::Module>("PinkREPL", *pink_context);

  pink_module->setTargetTriple(pink_target_triple);
  // all that work above just to validly call this function.
  pink_module->setDataLayout(pink_target_machine->createDataLayout());

  llvm::orc::JITTargetMachineBuilder pink_jit_target_machine_builder(pink_target_triple);
  llvm::orc::LLJITBuilder pink_lljit_builder;

  pink_lljit_builder.setJitTargetMachineBuilder(pink_jit_target_machine_builder);

  llvm::Expected<std::unique_ptr<llvm::orc::LLJIT>> pink_lljit = pink_lljit_builder.create();

  if (!pink_lljit)
  {
    //return error
  }

  /*
    build AST,
    convert AST to llvm IR
    add that IR to a new module,
    add that new module to the LLJIT
    lookup the symbol that was just defined,
     or lookup the temporary anonymous function
     being used to represent the local evaluation
     of some arbitrary code, i.e. what a REPL emulates.
    evaluate it,
     if it was an anonymous evaluation we are free to
     delete the symbols. however, we could also register
     their anonymous names, (and the actual names) into
     a big table which could be used for history.
    return the result
    print the result,
    ready for next line of input,
    repeat.


    i understand building the JIT, reading user input,
    parsing to the AST, building the LLVM IR represenation,
    adding that new IR to a new module for each statement,
    appending that module onto the LLJIT.
    and even looking up that symbol using the defined name,
    which could be the result of the user's bind expression,
    or it could be the result of the anonymous symbol naming
    of assorted statements. (like when the user just types
    x + y, or 2 * z.)

    however, LLJIT returns the address of the symbol only.
    which is fine, we can cast the address to a type, if
    we know it. so, when we access a value that we know to
    be Boolean, when we get the address from the JIT we cam
    cast the address to be the address of a boolean value,
    unwrap it, and boom we have the value to return or
    modify as we wish. we just need a function that
    brings those two facts together, and performs the
    cast, to get the correct pointer type out.
    this solution also tracks for integers, and Nil.
    our only other primitive types. however, what about when we want
    to call a function from our newly JIT'ed code, that
    was previously defined and thus is living in the
    LLJIT module? I guess what we want to build is something that
    calls an IFunction, and then the linker resolves the
    call once we add it to the JIT. so, our part of the
    program needs to keep the mapping of function names to
    function types, and from literal names to literal types.

    i guess we need to maintain the function definitions
    ourselves and use those values to build function
    calls, which are then resolved by the linker.
    the codgen procedure uses the name of the defined
    procedure in the call instruction, and the LLVM
    backed knows how to replace that name with the
    runtime address of the procedure which is resolved
    by us adding the procedure to the JIT. so we essentially
    have to maintain our symboltable to be identical to the
    symboltable of the JIT module.

    that is acceptable to get this working.

    so for further steps,

    loop:
    1) parse user input into an AST
    2) Getype of the AST for some basic typechecking
      and to annotate the AST with the LLVM::Type* representing
      the type of the term itself. (as each well formed term
      has one resulting type.)
    3) Codegen the AST to get a llvm representation.
    3a) if the term is a bind expression, then we are
        building a named binding within our symboltable,
        and since this is a REPL within the symbol table of
        the runtime, that is LLJIT.
        top level bind expressions are always declaring
        module global variables. which can then be constant
        or not, but thats later.
        in this situation we are building a named object
        with the type and value of the right hand side.

    3b) if a term is anything else, we are going to want to
        evaluate it, print the resulting value, and then
        throw away the result.

    3b1) when we say the words, evaluate, what we need to
         do eventually is 'call' the JIT'ed code like a
         procedure, as the JIT is going to return us, in
         essence, a basic block representing the term.
         so we are going to treat that basic block as if
         it were a no argument procedure which returns
         the type we both know about. (we called Getype
         remember?)
         this parameterless call is what brings the
         flow of control of pink into the control flow
         dictated by the input term. (if we were feeling
         fancy, we can place this transfer of the flow of control on another thread.
         and do 'remote' evaluation. but this is v1)

    4) so, when we do the bind, actually we are still doing
       this same transfer of the flow of control, it's just
       to evaluate the right hand side of the bind expression instead of
       the term itself. and instead of throwing away the result
       value, we store it in a 'global' variable. (function's
       work very differently, as their callble definitions only ever
       exist in the linker, what we can store into the global variable
       is a function-object, which also provides the facilities to
       hold a buffer of pre-bound variables. which allows the object
       to represent a delay of evaluation.

    4a) and we can consider building a call to a function known to
        us by the name being lookupable (as the variable would be
        bound to the function definition in our symbol table.
        literally a llvm::Value*, which is actually a llvm::Function*),
        or in the case that we encounter a function-object we could imagine calling
        the function which is pointed to by the functiion object.
        the type of the function object will have to be derived from
        the function it is wrapping, and then will be used for typechecking
        a call expression which uses said function object, so by the
        time we are generating code, we should already know that the
        type's match, and that the argument list is exactly enough
        to call the procedure, which results in the construction
        of the return type, or the return type is itself a function type,
        and we just want to store the value of the the new argument, appended
        to the set of old arguments, and build a new partial application
        which is N many arguments larger than the one that we built it
        out of, where N is the number of arguments provided to the
        application term which created the partial application.
        it's always the same underlying function pointer remember,
        when we apply the correct number of remaining arguments,
        (either a:the exact number of formal arguments.
         or     b:the remaining number of formal arguments,
                   once we consider the set of cached arguments
                   as bound within the function type.)
        we can construct a full function call. using the pointer
        as the address.
        (a function pointer is a llvm::PointerType, derived type instance,
        which is constructed passing in an instance of a FunctionType.
        the constructor can either take an address or no.)

*/

















  return 0;
}
