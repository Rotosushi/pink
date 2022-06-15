#pragma once 
#include <utility>
#include <vector>

#include "aux/StringInterner.h"
#include "aux/SymbolTable.h"
#include "ast/Ast.h"

namespace pink {
	/*
		So, it might be easier to get functions working 
		how llvm already wants them to work, and then modify 
		them to have the singular backend type, then implement 
		pointers and dynamic memory, and then 
		to implement lambdas on top of all that work. Because 
		all of that is required to get lambdas working regardless,
		and it does not force me into thinking of the whole
		solution all up front. (which should also help me stay motivated)
	*/
	class Function : public Ast {
	private:
		virtual Outcome<Type*, Error> GetypeV(std::shared_ptr<Environment> env) override;
	
	public:
		InternedString name;
		std::vector<std::pair<InternedString, Type*>> arguments;
		std::unique_ptr<Ast> body;
    std::shared_ptr<SymbolTable> bindings;
		
		Function(Location l, 
				 InternedString n, 
				 std::vector<std::pair<InternedString, Type*>> a, 
				 std::unique_ptr<Ast> b, 
				 SymbolTable* p);
				 
		virtual ~Function();
		
		virtual std::unique_ptr<Ast> Clone() override;
		
		static bool classof(const Ast* a);
		
		virtual std::string ToString() override;
		
		
		virtual Outcome<llvm::Value*, Error> Codegen(std::shared_ptr<Environment> env) override;
	};
}
