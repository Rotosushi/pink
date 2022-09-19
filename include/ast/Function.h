/**
 * @file Function.h
 * @brief Header for class Function
 * @version 0.1
 */
#pragma once 
#include <utility>
#include <vector>

#include "aux/StringInterner.h"
#include "aux/SymbolTable.h"
#include "ast/Ast.h"

namespace pink {
	/**
	 * @brief Represents an instance of a Function expression
	 * 
	 */
	class Function : public Ast {
	private:
		/**
		 * @brief Compute the Type of this Function
		 * 
		 * @param env the environment of this compilation unit
		 * @return Outcome<Type*, Error> if true the Type of this Function,
		 * if false the Error encountered.
		 */
		virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;
	
	public:
		/**
		 * @brief The name of this Function
		 * 
		 */
		InternedString name;

		/**
		 * @brief The formal arguments of this Function
		 * 
		 */
		std::vector<std::pair<InternedString, Type*>> arguments;

		/**
		 * @brief The Body expression of this Function
		 * 
		 */
		std::unique_ptr<Ast> body;

		/**
		 * @brief The local scope of this Function
		 * 
		 */
    	std::shared_ptr<SymbolTable> bindings;
		
		/**
		 * @brief Construct a new Function
		 * 
		 * @param loc the textual location of this Function
		 * @param name the name of this Function
		 * @param args the formal arguments to this Function
		 * @param body the Body expression of this Function
		 * @param outer_scope the Scope this Function resides within
		 */
		Function(Location loc, 
				 InternedString name, 
				 std::vector<std::pair<InternedString, Type*>> args, 
				 std::unique_ptr<Ast> body, 
				 SymbolTable* outer_scope);
	
		/**
		 * @brief Destroy the Function
		 * 
		 */
		virtual ~Function();
		
		/**
		 * @brief Implements LLVM style [RTTI] for this class
		 * 
		 * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
		 * 
		 * @param ast the ast to test
		 * @return true if ast *is* an instance of Function
		 * @return false if ast *is not* an instance of Function
		 */
		static bool classof(const Ast* ast);
		
		/**
		 * @brief Compute the cannonical string representation of this Function
		 * 
		 * @return std::string the string representation
		 */
		virtual std::string ToString() override;
		
		/**
		 * @brief Compute the value of this Function
		 * 
		 * @param env the environment of this compilation unit
		 * @return Outcome<llvm::Value*, Error> if true the llvm::Function representing this Function,
		 * if false the Error encountered.
		 */
		virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
	};
}
