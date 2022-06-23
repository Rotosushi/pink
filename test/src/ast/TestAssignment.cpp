#include <memory>

#include "Test.h"
#include "ast/TestAssignment.h"
#include "ast/Assignment.h"
#include "ast/Variable.h"
#include "ast/Nil.h"

#include "aux/Environment.h"

bool TestAssignment(std::ostream& out)
{
	bool result = true;
	out << "\n-----------------------\n";
  out << "Testing pink::Assignment: \n";
    
  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);  
  /*
    An Assignment term looks like 
    
    <assignable-term> '=' <value-term>
    e.x. "x = 7" or "y = nil"
  */
  
  pink::InternedString symb = env->symbols->Intern("z");
  pink::Location ll(0, 0, 0, 1);
  pink::Location rl(0, 4, 0, 7);
  pink::Location al(0, 0, 0, 7);
  std::unique_ptr<pink::Ast> var = std::make_unique<pink::Variable>(ll, symb);
  pink::Ast* var_p = var.get();
	std::unique_ptr<pink::Ast> nil = std::make_unique<pink::Nil>(rl);
	pink::Ast* nil_p = nil.get();
	
	llvm::Value* nil_v = env->builder->getFalse();
  pink::Type*  nil_t = env->types->GetNilType();
    
  env->bindings->Bind(symb, nil_t, nil_v);
	
	std::unique_ptr<pink::Assignment> ass = std::make_unique<pink::Assignment>(al, std::move(var), std::move(nil));
	
	result &= Test(out, "Assignment::getKind()", ass->getKind() == pink::Ast::Kind::Assignment);
	
	result &= Test(out, "Assignment::classof()", ass->classof(ass.get()));
	
	result &= Test(out, "Assignment::GetLoc()", ass->GetLoc() == al);
	
	result &= Test(out, "Assignment::left", ass->left.get() == var_p);
	
	result &= Test(out, "Assignment::right", ass->right.get() == nil_p);
	
	std::string ass_str = var_p->ToString() + " = " + nil_p->ToString();
	
	result &= Test(out, "Assignment::ToString()", ass->ToString() == ass_str);
	
	pink::Outcome<pink::Type*, pink::Error> ass_type = ass->Getype(env);
	
	result &= Test(out, "Assignment::Getype()", ass_type && ass_type.GetOne() == nil_t);
	
	result &= Test(out, "pink::Assignment", result);
  out << "\n-----------------------\n";
	return result;
}



