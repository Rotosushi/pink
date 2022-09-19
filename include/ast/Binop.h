/**
 * @file Binop.h
 * @brief Header for class Binop
 * @version 0.1
 */
#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
    /**
     * @brief Represents an instance of a binary operator expression
     * 
     */
    class Binop : public Ast {
    private:
      /**
       * @brief Compute the Type of this Binop expression
       * 
       * the type of a binop expression is the return type after applying the binop.
       * 
       * @param env the environment of this compilation unit
       * @return Outcome<Type*, Error> if true the type of the binop expression,
       * if false the Error encountered.
       */
    	virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;
    
    public:
      /**
       * @brief The binary operator of this expression
       * 
       */
      InternedString       op;

      /**
       * @brief the left hand side of the binop expression
       * 
       */
      std::unique_ptr<Ast> left;

      /**
       * @brief the right hand side of the binop expression
       * 
       */
      std::unique_ptr<Ast> right;

      /**
       * @brief Construct a new Binop
       * 
       * @param loc the textual location of this binop expression
       * @param op the operator of the binop expression
       * @param left the left hand side of the binop expression
       * @param right the right hand side of the binop expression
       */
      Binop(Location& loc, InternedString op, std::unique_ptr<Ast> left, std::unique_ptr<Ast> right);
      
      /**
       * @brief Destroy the Binop
       * 
       */
      virtual ~Binop();

      /**
       * @brief Implements LLVM style [RTTI] for this class
       * 
       * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
       * 
       * @param ast the ast to test
       * @return true if ast *is* an instance of a Binop
       * @return false if ast *is not* an instance of a Binop
       */
      static bool classof(const Ast* ast);

      /**
       * @brief Compute the cannonical string representation of the Binop expression
       * 
       * @return std::string the string representation
       */
      virtual std::string ToString() override;

      /**
       * @brief Compute the Value of this Binop expression
       * 
       * the value is the result value after applying the binop to the 
       * value of the left and right sides.
       * 
       * @param env the environment of this compilation unit
       * @return Outcome<llvm::Value*, Error> if true the value after applying the binop,
       * if false the Error encountered.
       */
      virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
    };
}
