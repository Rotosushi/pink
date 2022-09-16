#pragma once 
#include <vector>

#include "aux/SymbolTable.h"

#include "ast/Ast.h"


namespace pink {
	class Block : public Ast {
	private:
		virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;
	
	public:
		std::vector<std::unique_ptr<Ast>> statements;
		
		typedef typename std::vector<std::unique_ptr<Ast>>::iterator iterator;
		
		
		Block(Location loc);
		Block(Location loc, std::vector<std::unique_ptr<Ast>>& stmnts);
		~Block();
		
		iterator begin();
		iterator end();
		
		static bool classof(const Ast* ast);
		
		virtual std::unique_ptr<Ast> Clone() override;
		virtual std::string ToString() override;
		
		
		virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
	};
}
