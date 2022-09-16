#pragma once
#include <vector>

#include "ast/Ast.h"


/*
 *  Represents a tuple of values.
 *  it can also be thought of as an anonymous
 *  struct type with numbered element access instead 
 *  of named element access.
 *
 *  the underlying data-type of structs and tuples are 
 *  going to be identical, the only real difference is that 
 *  the struct type holds a LUT mapping the names to the 
 *  element offset.
 *
 *  otherwise, they can be either single value sized, or 
 *  greater, and as such we must modify our functions 
 *  when necessesary to support returning them by value 
 *  from a function. this is only when the struct type 
 *  in question is larger than a single value type.
 *
 */

namespace pink {
  class Tuple : public Ast {
  private:
    virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;

  public:
    std::vector<std::unique_ptr<Ast>> members;
  
    Tuple(Location loc, std::vector<std::unique_ptr<Ast>> members);
    virtual ~Tuple();

    static bool classof(const Ast* ast);

    virtual std::unique_ptr<Ast> Clone() override;
    virtual std::string ToString() override;

    virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
  };
}
