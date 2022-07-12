
#include "Test.h"
#include "ast/TestApplication.h"
#include "ast/Application.h"
#include "ast/Function.h"
#include "ast/Variable.h"
#include "ast/Int.h"
#include "ast/Bool.h"

#include "aux/Environment.h"

bool TestApplication(std::ostream& out)
{
  bool result = true;
  out << "\n-----------------------------\n";
  out << "Testing pink::Application: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::NewGlobalEnv(options);

  // we need to define a function and then construct an application term
  // which calls the function.

  pink::Outcome<std::unique_ptr<pink::Ast>, pink::Error> fn = env->parser->Parse("fn add(x: Int, y: Int) { x + y }", env);

  // 'add 5 7'
  pink::Location idloc(1, 0, 1, 2);
  pink::Location arg0loc(1, 4, 1, 5);
  pink::Location arg1loc(1, 6, 1, 7);
  std::unique_ptr<pink::Ast> fnname = std::make_unique<pink::Variable>(idloc, env->symbols->Intern("add"));
  
  std::vector<std::unique_ptr<pink::Ast>> args;

  args.emplace_back(std::make_unique<pink::Int>(arg0loc, 5));
  args.emplace_back(std::make_unique<pink::Int>(arg1loc, 7));

  pink::Location apploc(1, 0, 1, 7);
  std::unique_ptr<pink::Application> app(new pink::Application(apploc, std::move(fnname), std::move(args)));

  // given an application term can we access the peripheral data?
  result &= Test(out, "Application::getKind()", app->getKind() == pink::Ast::Kind::Application);

  result &= Test(out, "Application::classof()", app->classof(app.get()));

  result &= Test(out, "Application::GetLoc()", app->GetLoc() == apploc);

  std::string appstr = "add 5 7";
  result &= Test(out, "Application::ToString()", app->ToString() == appstr);

  // given an application of a defined function, 
  // is the type correct?
  
  // so far, Applicatio::  iGetype will fail because we do not bind function
  // definitions to llvm::Value*'s which we could never do until we codegen
  // the function definition. now, this is a very interesting question,
  // because when are we supposed to codegen function definitions when trying 
  // to type a newly parsed block of statements? say for instance when
  // the body of some given procedure applies another function which we 
  // do not have the definition of yet within said block of statements?
  // THis is a case of use before definition, and makes the solution to this 
  // problem, equivalent to the solution of the following problem
  // x : Int = y + offset;
  // ...
  // y := 56; // or some other value
  //
  //
  // a solution might begin with adding a Type* to the SymbolTable
  // then, when we encounter a bind, we add the type as well.
  // then, when we encounter a function, we bind it to a type,
  // and a nullptr for the value. then we can fill in the value* 
  // after codegening the function. this way, even though we don't
  // have the function itself yet, we have it's type.
  // although, that just moves the problem into typeing an expression
  // which uses something that is not defined yet. like a function 
  // calling another function which has a definition later in the source.
  //
  // okay, so what about catching the specific error of use-before-definition,
  // and adding all of those to a table which points to the false symbol table 
  // entry that we make at each usage of a variable that we cannot find.
  // then, each definition of a new variable is checked against this table, and 
  // then we somehow define the variable that failed to be defined when we
  // caught the use-before-definition.
  //
  // whatever we implement has to make mutually recursive definitions viable as
  // well.
  //
  //
  // ideally we have a solution such that as long as the definition appears
  // somewhere in the source we are currently compiling (because we aren't
  // worrying about module inclusion just yet, but that is important)
  // then we can compile the source to machine code.
  //
  //
  //  in steps:
  //    when we encounter a variable declaration
  //    if the declaration is fully qualified, we simply act as normal
  //      and define said variable. except, we also check this new declaration 
  //      against the new table, and if we are declaraing a variable which is 
  //      needed to declare another variable, we then attempt to type the
  //      statement which is associated with that variable. Then, because there 
  //      could be more than one undefined variable within the declaration we 
  //      repeat this same procedure for that new try at the old declaration.
  //    if the declaration is unable to be typed because of
  //      a use-before-definition error then we create an empty 
  //      declaration and add it to a table. 
  //      (i think this just needs to be a 
  //      table of identifiers to identifiers, the undefined name is the key,
  //      the variable that would be defined, along with it's declaration string
  //      are the value.)
  //      then we move on to parse more input.
  //    if the declaration returns any other error we simply return that error.
  //
  //
  //  the table is composed of elements like 
  //
  //  InternedString undefname -> (InternedString newvar, std::unique_ptr<Ast> declast)
  //
  //  where the declast references undefname within the tree declaring
  //  newvar. so that we can use that to create the symbol table entry 
  //  for newvar.
  //
  //  within the scope of a function, use-before-definition is not allowed.
  //  within global scope, it is.
  //
  //  also this solution brings up the fact that declarations can occur across 
  //  multiple lines of source code, which would all need to be gathered into a
  //  single declast element to be typed later.
  //
  //  the real problem is that we currently parse and then attempt to type the 
  //  entire source file at once. while this isn't a problem in and of itself,
  //  what it means is that we do not have a moment where we can splice out 
  //  individual declarations to be filled in later.
  //
  //  this stems from our implementation of parsing blocks of code.
  //  that is how we handle source files which hold sequences of terms,
  //  and not simply single terms themselves. 
  //  we always try to parse the entire block all at once.
  //  if instead of ever parsing a 'block' of statments, we parsed 
  //  and subsequently typed each statement within the block, then 
  //  we would have a place where we could implement the above solution.
  //  Notice that should we handle 'blocks' of code in this way, we still 
  //  want to parse function bodies all at once, so if we exclusively use 
  //  our pink::Block to represent function bodies, then we can construct
  //  another structure (or simply define a std::vector<>) which represents
  //  the source file we are parsing itself. that is the sequence of
  //  declarations which occur in said source file.
  //
  //  and all of this is just for typing the out of order declaration,
  //  when we go to codegen anything which relies on an out of order
  //  declaration we will again have to handle that, except this time within
  //  the rules of llvm. which means i think that we need to emit function 
  //  definitions (function headers essentially) when we can type the function
  //  but not declare it yet, due to out of order declarations. then because we
  //  have the header then llvm can know that the linker will be able to splice
  //  in the actual function call during linking. then out of order
  //  declarations would be able to procede according to the steps above.
  //  
  //  so in steps again,
  //
  //  when we try to codgen a function whose body calls or uses an out of order
  //  declaration we still error out of creating that declaration, at that
  //  moment, however we store everything needed to retry the declaration in
  //  a table, where the key to retrieve the data is the name which we were
  //  missing the declaration of. 
  //  when we declare a new name and it is successfull we check this name
  //  against said table, and any declaration which is retrieveable against
  //  said name can be retried. any failures, by another use before definition
  //  error will be kept within the table, except retrievable by the new
  //  undeclared name. 
  //
  //
  //  if at the end of parsing the entirety of the source we still do not know
  //  the names within the declarations then each use-before-declaration will
  //  be raised to an actual error, and handled as we already handle errors.
  //
  //    
  //
  //
  pink::Type* int_type = env->types->GetIntType();
  pink::Outcome<pink::Type*, pink::Error> app_type = app->Getype(env);
  result &= Test(out, "Application::Getype()", app_type && app_type.GetOne() == int_type);

  // given an application of not a function,
  // do we get an error?
  
  // given an application of a function given the 
  // wrong number of arguments,
  // do we get an error?
  
  // given an application of a function given the 
  // wrong type of arguments,
  // do we get an error?


  result &= Test(out, "pink::Application", result);
  return result;
}
