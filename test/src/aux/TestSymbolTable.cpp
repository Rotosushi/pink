#include "Test.h"
#include "aux/TestSymbolTable.h"
#include "aux/SymbolTable.h"

#include "aux/Environment.h"

#include "ast/Nil.h"


bool TestSymbolTable(std::ostream& out)
{
    /*
        Testing the various functions of the symbol table entails:

        -) SymbolTable's constructed against no outer scope have
            a nullptr for their OuterScope

        -) SymbolTable's Constructed 'inside' of another symbol table
            have a valid pointer to that symbol table

        -) Bind validly binds a symbol to a particular term because
            -) Lookup of an existing symbol returns that symbols binding
            
        #TODO
        -) Lookup finds symbols bound in the scope above, a property of
        	Lexical scoping.
        #TODO
        -) A local lookup will pnly search for names bound within the 
        	current scope, so we can tell the difference between 
        	free and bound names within scopes.

        -) Unbind successfully removes a particular binding because
            -) Lookup of a nonexisting symbol returns the empty optional

    */

    bool result = true;
    out << "\n-----------------------\n";
    out << "Testing pink::SymbolTable: \n";

    auto options = std::make_shared<pink::CLIOptions>();
    auto env = pink::NewGlobalEnv(options);

    pink::SymbolTable t1(env->bindings.get());

    result &= Test(out, "SymbolTable::OuterScope(), global scope", env->bindings->OuterScope() == nullptr);

    result &= Test(out, "SymbolTable::OuterScope(), inner scope", t1.OuterScope() == env->bindings.get());

    pink::Location l(0, 5, 0, 7);
    llvm::Value* nil = env->instruction_builder->getFalse();
    pink::Type*  nil_t = env->types->GetNilType();
    pink::InternedString x = env->symbols->Intern("x");

    env->bindings->Bind(x, nil_t, nil);
    llvm::Optional<std::pair<pink::Type*, llvm::Value*>> s0 = env->bindings->Lookup(x);

    result &= Test(out, "SymbolTable::Bind()", s0.hasValue());
    result &= Test(out, "SymbolTable::Lookup()", (*s0).first == nil_t && (*s0).second == nil);

    env->bindings->Unbind(x);
    llvm::Optional<std::pair<pink::Type*, llvm::Value*>> s1 = env->bindings->Lookup(x);

    result &= Test(out, "SymbolTable::Unbind()", !s1.hasValue());

    result &= Test(out, "pink::SymbolTable", result);
    out << "\n-----------------------\n";
    return result;
}
