#pragma once 
#include <string>

#include "type/Type.h"
/*
   *
   * So there are a few points of confusion for pointers.
   *
   * 1) what is a pointer in LLVM?
   *  -) that is actually easy to answer, a pointer in LLVM is just like a 
   *      pointer in C. and, in fact we are already using them within the 
   *      language within the Variable, and VarRef structures. That is 
   *      because CreateAlloca, and CreateGlobal return pointers to the 
   *      memory they allocate on the stack or in global memory respectively.
   *      this means that when Variable is bound to a Value*, that Value* is 
   *      either a Pointer to memory, or a Constant value directly.
   *      this means that we can simply construct Pointers using the same
   *      information that we use to construct the Value* that a Variable is 
   *      bound to.
   *    this however raises our second question:
   *  2) How can we store and load pointer values into memory in LLVM?
   *     that is, if we are already using Pointers to properly Load/Store in the 
   *     language, how can we modify the Load/Store process to store the 
   *     pointer itself, instead of the data it is pointing to?
   *     -) the answer to that is with the PtrToInt, and IntToPtr instructions 
   *          which are provided by LLVM, with the PtrToInt instruction we can convert 
   *          what is normaly a pointer value into an integer value, and then 
   *          we can load/store that integer like we would any other integer. 
   *          and conversely, given an integer value associated with a pointer 
   *          type we can use IntToPtr to retreive the Pointer object such that 
   *          we can Load/Store the data which the pointer is referencing.
   *
   *          in this way PtrToInt acts in a manner that makes it good for
   *          implementing the address of value operator (unary &),
   *          and IntToPtr acts in a manner that makes it good for implementing
   *          the value of address operator (unary * ).
   *
   *  This is what leads us to our solution, namely, Pointers do not exist in
   *  the structure of the terms in the language, that is as an Ast node, but 
   *  as values which the Ast nodes are constructing/destructing.
   *
   *  int main() {
   *    x := 0;
   *    y := &x;
   *    *y = 5;
   *    x;
   *  } 
  */

namespace pink {
  class PointerType : public Type {
  public:
    Type* pointee_type;

    PointerType(Type* pointee_type);
    virtual ~PointerType();

    static bool classof(const Type* t);

    virtual bool EqualTo(Type* other) override;
    virtual std::string ToString() override;

    virtual Outcome<llvm::Type*, Error> Codegen(std::shared_ptr<Environment> env) override;
  };
}
