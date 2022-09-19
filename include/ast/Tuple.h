/**
 * @file Tuple.h
 * @brief Header for class Tuple
 * @version 0.1
 */
#pragma once
#include <vector>

#include "ast/Ast.h"

namespace pink {
  /**
   * @brief Represents a Tuple expression
   * 
   * 
   *  can also be thought of as an anonymous
   *  struct type with numbered element access instead 
   *  of named element access.
   *
   *  the underlying data-type of structs and tuples are 
   *  going to be identical, the only real difference is that 
   *  the struct type holds a LUT mapping the names to the 
   *  element offset.
   *
   *  elements can be either single value sized, or 
   *  greater, and as such we must modify a Function 
   *  when necessesary to support returning them by value 
   *  from a Function or passing them by value to a Function.
   */
  class Tuple : public Ast {
  private:
    /**
     * @brief Compute the Type of this Tuple
     * 
     * @param env the environment of this compilation unit
     * @return Outcome<Type*, Error> if true the Type of this Tuple,
     * if false the Error encountered.
     */
    virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;

  public:
    /**
     * @brief The members of this Tuple
     * 
     */
    std::vector<std::unique_ptr<Ast>> members;
  
    /**
     * @brief Construct a new Tuple
     * 
     * @param loc the textual location of this Tuple
     * @param members the member expressions of this Tuple
     */
    Tuple(Location loc, std::vector<std::unique_ptr<Ast>> members);

    /**
     * @brief Destroy the Tuple
     * 
     */
    virtual ~Tuple();

    /**
     * @brief Implements LLVM style [RTTI] for this class
     * 
     * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
     * 
     * @param ast the ast being tested
     * @return true if ast *is* an instance of Tuple
     * @return false if ast *is not* an instance of Tuple
     */
    static bool classof(const Ast* ast);

    /**
     * @brief Compute the cannonical string representation of this Tuple
     * 
     * @return std::string the string representation
     */
    virtual std::string ToString() override;

    /**
     * @brief Compute the value of this Tuple
     * 
     * Due to a language limitation Tuple's may only be constructed 
     * from constant members.
     * 
     * @param env the environment of this compilation unit
     * @return Outcome<llvm::Value*, Error> if true an llvm::ConstantStruct,
     * if false the Error encountered
     */
    virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
  };
}
