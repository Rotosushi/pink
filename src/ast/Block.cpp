
#include "ast/Block.h"
#include "aux/Environment.h"

namespace pink {
	Block::Block(const Location& location)
		: Ast(Ast::Kind::Block, location)
	{
	
	}
	
	Block::Block(const Location& location, std::vector<std::unique_ptr<Ast>>& stmnts)
		: Ast(Ast::Kind::Block, location), statements(std::move(stmnts))
	{
	
	}
	
	auto Block::begin() const -> Block::const_iterator
	{
		return statements.begin();
	}
	
	auto Block::end() const -> Block::const_iterator
	{
		return statements.end();
	}
	
	auto Block::classof(const Ast* ast) -> bool
	{
		return ast->getKind() == Ast::Kind::Block;
	}
	
	auto Block::ToString() const -> std::string
	{
		std::string result("{ ");
		
		// #TODO: Make this prepend the correct number of 
		// 			tabs to properly indent the block at 
		//			the current level of nesting.
		for (const auto& stmt : statements)
		{
			result += stmt->ToString() + ";\n";
		}

    	result += " }";
		
		return result;
	}
	
	/*
		The type of a block is the type of it's last statement.
		
		env |- s0 : T0, s1 : T1, ..., sn : Tn
	----------------------------------------------
			env |- s0; s1; ...; sn; : Tn
	
	*/
	auto Block::GetypeV(const Environment& env) const -> Outcome<Type*, Error>
	{
		Outcome<Type*, Error> result(Error(Error::Code::None, loc));
		
		for (const auto& stmt : statements)
		{
			result = stmt->Getype(env);
			
			if (!result)
			{
				return result;
			}
		}

		return result;
	}


	/*
		The value of a block is the value of it's last statement.
	*/
	auto Block::Codegen(const Environment& env) const -> Outcome<llvm::Value*, Error>
	{
		Outcome<llvm::Value*, Error> result(Error(Error::Code::None, loc));
		
		for (const auto& stmt : statements)
		{
			result = stmt->Codegen(env);
			
			if (!result)
			{
				return result;
			}
		}
		
		return result;
	}
}
