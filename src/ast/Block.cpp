
#include "ast/Block.h"
#include "aux/Environment.h"

namespace pink {
	Block::Block(Location loc)
		: Ast(Ast::Kind::Block, loc)
	{
	
	}
	
	Block::Block(Location loc, std::vector<std::unique_ptr<Ast>>& stmnts)
		: Ast(Ast::Kind::Block, loc), statements(std::move(stmnts))
	{
	
	}
	
	Block::~Block()
	{
	
	}
	
	Block::iterator Block::begin()
	{
		return statements.begin();
	}
	
	Block::iterator Block::end()
	{
		return statements.end();
	}
	
	bool Block::classof(const Ast* ast)
	{
		return ast->getKind() == Ast::Kind::Block;
	}
	
	std::unique_ptr<Ast> Block::Clone()
	{
		std::vector<std::unique_ptr<Ast>> stmnts;
		
		for (auto& stmt : statements)
		{
			stmnts.emplace_back(stmt->Clone());
		}
		
		return std::make_unique<Block>(loc, stmnts);
	}
	
	std::string Block::ToString()
	{
		std::string result("");
		
		// #TODO: Make this prepend the correct number of 
		// 			tabs to properly indent the block at 
		//			the current level of nesting.
		for (auto& stmt : statements)
		{
			result += stmt->ToString() + ";\n";
		}
		
		return result;
	}
	
	/*
		The type of a block is the type of it's last statement.
		
		env |- s0 : T0, s1 : T1, ..., sn : Tn
	----------------------------------------------
			env |- s0; s1; ...; sn; : Tn
	
	*/
	Outcome<Type*, Error> Block::GetypeV(std::shared_ptr<Environment> env)
	{
		Outcome<Type*, Error> result(Error(Error::Kind::Default, "Default Error", loc));
		
		for (auto& stmt : statements)
		{
			result = stmt->Getype(env);
			
			if (!result)
				return result;
		}
		
		// calling Getype on any bind expressions within this block 
		// will have created false bindings, such that later expressions
		// using the previous ones will be able to be properly bound.
		// so we clean those up here before returning the final type.
		for (InternedString fbnd : env->false_bindings)
		{
			env->bindings->Unbind(fbnd);
		}
		// since we removed them, clear them from the list.
		env->false_bindings.clear();
		
		return result;
	}


	/*
		The value of a block is the value of it's last statement.
	*/
	Outcome<llvm::Value*, Error> Block::Codegen(std::shared_ptr<Environment> env)
	{
		Outcome<llvm::Value*, Error> result(Error(Error::Kind::Default, "Default Error", loc));
		
		for (auto& stmt : statements)
		{
			result = stmt->Codegen(env);
			
			if (!result)
				return result;
		}
		
		return result;
	}
}
