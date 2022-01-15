

#include "Test.h"
#include "ast/TestAssignment.h"
#include "ast/Assignment.h"
#include "ast/Variable.h"
#include "ast/Nil.h"

bool TestAssignment(std::ostream& out)
{
	bool result = true;
	out << "\n-----------------------\n";
    out << "Testing pink::Assignment: \n";
    
    /*
    	An Assignment term looks like 
    	
    	<assignable-term> '=' <value-term>
    	e.x. "x = 7" or "y = nil"
    */
    
    pink::StringInterner strs;
    pink::InternedString symb = strs.Intern("z");
    pink::Location ll(0, 0, 0, 1);
    pink::Location rl(0, 4, 0, 7);
    pink::Location al(0, 0, 0, 7);
    pink::Variable* var = new pink::Variable(ll, symb);
	pink::Nil* nil = new pink::Nil(rl);
	pink::Assignment* ass = new pink::Assignment(al, var, nil);
	
	result &= Test(out, "Assignment::getKind()", ass->getKind() == pink::Ast::Kind::Assignment);
	
	result &= Test(out, "Assignment::classof()", ass->classof(ass));
	
	result &= Test(out, "Assignment::GetLoc()", ass->GetLoc() == al);
	
	result &= Test(out, "Assignment::left", ass->left == var);
	
	result &= Test(out, "Assignment::right", ass->right == nil);
	
	std::string ass_str = var->ToString() + " = " + nil->ToString();
	
	result &= Test(out, "Assignment::ToString()", ass->ToString() == ass_str);
	
	result &= Test(out, "pink::Assignment", result);
    out << "\n-----------------------\n";
	return result;
}



