#include "kernel/Cast.h"

#include "llvm/IR/DerivedTypes.h"

namespace pink {

  Outcome<llvm::Value*, Error> Cast(llvm::Value* value, llvm::Type* target_type, std::shared_ptr<Environment> env)
  {
    llvm::Type* value_type = value->getType();
    
    // so based on the to_type and the from_type we need to choose our cast instruction.
    // to tell what the to type is exactly we can use either llvm::isa or
    // llvm::dyn_cast, however since we probably want to use the information
    // held within the to type, this means we want dyn_cast.
    //
    // there are a few main types which we want to be able to cast between,
    // integers
    // pointers
    // floating point types
    //
    //
    if (llvm::IntegerType* from_type = llvm::dyn_cast<llvm::IntegerType>(value_type))
    {
      // so there are a few main cast operations on integer types
      // an int type cast to another int type 
      // an int type cast to a float type
      // an int type cast to a pointer type
      //
      if (llvm::IntegerType* to_type = llvm::dyn_cast<llvm::IntegerType>(target_type))
      {
        unsigned from_bitwidth = from_type->getBitWidth();
        unsigned to_bitwidth   = to_type->getBitWidth();
       
        // if the value being extended is a positive integer, then the sign bit
        // is zero and a zext will result in the correct number in the resulting 
        // value. however when we zext a signed negative number, this will
        // result in a positive number in the resulting value, which would
        // be a semantic error. however, we can't know the runtime value of
        // the given llvm::Value in every possible case! so how do we handle
        // this? as of right now, there are only booleans and signed integers,
        // and we are only trying to convert from boolean to signed integer, 
        // so zext works great, as false gets correctly converted to signed
        // zero of the corresponding length, and true gets correctly converted 
        // to positive 1 of the corresponding length.
        
        // any unsigned, or positive signed integer can be zero extended to
        // a larger bitwidth with no loss of information, however since signed 
        // integers can also be negative, it is better to only emit sext
        // instructions. however, since llvm uses no such thing as signed or 
        // unsigned integers, we need to keep track of that with our type
        // system instead. This means we need to pass in the pink::Type
        // of the value, so we can know to emit a zext or sext for integer 
        // conversion.
        if (from_bitwidth < to_bitwidth)
        {
          return Outcome<llvm::Value*, Error>(env->builder->CreateZExt(value, target_type, "zext"));
        }
        else if (from_bitwidth > to_bitwidth)
        {
          return Outcome<llvm::Value*, Error>(env->builder->CreateTrunc(value, target_type, "trunc"));
        }
        else // from_bitwidth == to_bitwidth 
        {
          return Outcome<llvm::Value*, Error>(env->builder->CreateBitCast(value, target_type, "bitcast"));
        }
      }
      //else if (llvm::PointerType* to_type = llvm::dyn_cast<llvm::PointerType>(target_type))
      //else if (to_type->isFloatingPointTy());
      else 
      {
        return Outcome<llvm::Value*, Error>(Error(Error::Code::CannotCastToType, Location()));
      }
    }
    // else if (llvm::PointerType* from_type = llvm::dyn_cast<llvm::PointerType>(target_type))
    // else if (from_type->isFloatingPointTy())
    else
    {
      return Outcome<llvm::Value*, Error>(Error(Error::Code::CannotCastFromType, Location()));
    }
  }

}
