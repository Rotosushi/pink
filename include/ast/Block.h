/**
 * @file Block.h
 * @brief Header for class Block
 * @version 0.1
 */
#pragma once 
#include <vector>

#include "aux/SymbolTable.h"

#include "ast/Ast.h"


namespace pink {
	/**
	 * @brief Represents a Block of expressions
	 * 
	 */
	class Block : public Ast {
	private:
		/**
		 * @brief Compute the Type of the block expression
		 * 
		 * the result type of a block is the type of the last statement in
		 * the sequence.
		 * 
		 * @param env the environment of this compilation unit
		 * @return Outcome<Type*, Error> if true the return type of the block expression,
		 * if false the Error encountered.
		 */
		virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;
	
	public:
		/**
		 * @brief the sequence of statements composing the block
		 * 
		 */
		std::vector<std::unique_ptr<Ast>> statements;
		
		/**
		 * @brief an iterator over the statements within the block
		 * 
		 */
		typedef typename std::vector<std::unique_ptr<Ast>>::iterator iterator;
		
		/**
		 * @brief Construct a new empty Block
		 * 
		 * @param loc the textual location of the block
		 */
		Block(Location loc);

		/**
		 * @brief Construct a new Block
		 * 
		 * @param loc the textual location of the block
		 * @param stmnts the statements composing the block
		 */
		Block(Location loc, std::vector<std::unique_ptr<Ast>>& stmnts);

		/**
		 * @brief Destroy the Block
		 * 
		 */
		~Block();
		
		/**
		 * @brief Get the iterator to the beginning of the block
		 * 
		 * @return iterator the iterator to the beginning of the block
		 */
		iterator begin();

		/**
		 * @brief Get the iterator to the end of the block
		 * 
		 * @return iterator the iterator to the end of the block
		 */
		iterator end();
		
		/**
		 * @brief Implements LLVM style [RTTI] for this class
		 * 
		 * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
		 * 
		 * @param ast the ast to test
		 * @return true if ast *is* an instance of a Block
		 * @return false if ast *is not* an instance of a Block
		 */
		static bool classof(const Ast* ast);
		
		/**
		 * @brief Return the cannonical string representation of the Block
		 * 
		 * @return std::string the string representation
		 */
		virtual std::string ToString() override;
		
		/**
		 * @brief Compute the Value of this Block
		 * 
		 * The result Value of a Block is the result value of the last statement within the Block
		 * 
		 * @param env the environment of this compilation unit
		 * @return Outcome<llvm::Value*, Error> if true the result value of the Block,
		 * if false the Error encountered.
		 */
		virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
	};
}
