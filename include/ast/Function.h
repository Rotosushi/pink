#pragma once 
#include <utility>
#include <vector>

#include "aux/StringInterner.h"
#include "aux/SymbolTable.h"
#include "ast/Ast.h"

namespace pink {
	
	class Function : public Ast {
	private:
		virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;
	
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
		
		
		virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
	};
}
