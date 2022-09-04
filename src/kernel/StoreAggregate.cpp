#include "kernel/StoreAggregate.h"


namespace pink {
  /*
   *
   *  StoreAggregate is meant to be a function which can handle storing types
   *  which are larger than a single value. this is fundamentally not that 
   *  hard to do, as we can simply emit regular llvm StoreInstructions for 
   *  each single value element of a given aggregate. the problem that cannot 
   *  be easily solved in a single scope is the case when the element of a 
   *  particular aggregate is itself not a single value type. In this case we 
   *  must loop again to store each single value sized element of that element 
   *  of the original aggregate. This function can do that by calling itself 
   *  on the element of the aggregate, thus recursively handling the solution 
   *  for any nested depth of aggregate storage. 
   *
   *  there are two distinct cases that cannot be mixed however, and that is 
   *  dependant upon the kind of source we are given. we can also use this 
   *  procedure upon Constant's, thus making this procedure equivalent to 
   *  a constant initializer procedure. Now this is not useful for initializing 
   *  GlobalVariables, as they can already be given aggregate llvm::Constant*
   *  as initializers, but it does come in handy when considering initializing 
   *  a local variable with a constant initializer, or when copying an existing
   *  aggregate to another memory location.
   *  
   *
   *  instead of spliting this function in two internally with a huge 
   *  if ... else ... clause, we are going to split the procedure into 
   *  two procedures. one for storing a llvm::Constant* Aggregate into 
   *  a memory location, and one for storing the contents of one memory 
   *  location into another. Hold up, can't we support that with a memcpy?
   *  I think so, yeah, we just need the sizes of the types involved.
   *
   *  Input Constraints: 
   *   -) type of src and dest must be equivalent, and each must have type 
   *      'ty'.
   *
   *   -) ty must be the type of dest in memory, and dest must be
   *      a pointer to an allocation of memory large enough to hold 
   *      a type 'ty'. this is to support constructing GEP 
   *      instructions directly from dest, ty.
   *   -) ty must be the type of src in memory, and src must be 
   *      a pointer to an allocation of memory large enough to hold
   *      a type ty, and already holding the values wanted in dest.
   *      OR it could be an llvm::Constant* holding the initialization 
   *      values for dest.
   *   
   *   -) env must be set up such that the ir builder is in the correct
   *      position to emit the loads and stores for the values.
   *      this procedure will not modify the insertion position of the 
   *      builder.
   *
   *   Output constraints:
   *    -) this function returns dest, that is it returns a pointer to the 
   *       destination -after- performing the store instructions. 
   *       in this way it should be acceptable to compute GEP instructions 
   *       from the return value of this procedure such that the elements 
   *       of dest can again be referenced within the program. 
   *       This is only viable because the store instructions peformed 
   *       upon dest, have the side effect of modifying the memory at that 
   *       location. so a pointer to that memory location need not be updated 
   *       to reflect those changes. because it does naturally so by being a
   *       referenc to the memory. 
   *
   *
   */
  llvm::Value* StoreAggregate(llvm::Type* ty, llvm::Value* dest, llvm::Value* src, std::shared_ptr<Environment> env)
  {
    if (llvm::Constant* const_src = llvm::dyn_cast<llvm::Constant>(src))
    {
      return StoreConstAggregate(ty, dest, const_src, env);
    }
    else
    {
      return StoreValueAggregate(ty, dest, src, env);
    }
  }

  llvm::Value* StoreConstAggregate(llvm::Type* ty, llvm::Value* dest, llvm::Constant* src, std::shared_ptr<Environment> env)
  {
    if (llvm::ArrayType* at = llvm::dyn_cast<llvm::ArrayType>(ty))
    {
      llvm::ConstantArray* src_array = llvm::cast<llvm::ConstantArray>(src);
      if (src_array == nullptr)
      {
        FatalError("Array Type given to StoreConstAggregate, but source is not a ConstantArray", __FILE__, __LINE__);
      }
      // so, if the element type of the array is a single value type,
      // then we can safely use a llvm::StoreInstruction to perform the 
      // store. In the case where the element is itself an aggregate 
      // then for each element we want to store we recursively call this
      // procedure to store it.
      if (at->getElementType()->isSingleValueType())
      {
        size_t length = at->getNumElements();
        for (size_t i = 0; i < length; i++)
        {
          // from reading the Docs here: llvm.org/docs/GetElementPtr.html
          // the GEP instruction takes the type of the element being indexed 
          // over, which is dependent upon the 
          // element type of the array being stored. the next argument 
          // is the pointer to the array to compute the offset relative of,
          // the third argument is an index over the type given. so 0 will be 
          // the array pointed to by ptr, 1 would be the pointer to the same 
          // type of array, but appearing after the end of the given array,
          // as if we had stored an array of arrays, this would be the second 
          // array member, 3 would be the third array, and so on and so
          // forth. the fourth argument is the index into the type given,
          // so this would be an element of the array given, the first elem 
          // is at offset 0, then offset 1 is the second element and so on. 
          llvm::Value* elemPtr = env->builder->CreateConstGEP2_32(at, dest, 0, i);
          env->builder->CreateStore(src_array->getAggregateElement(i), elemPtr);
        }
      }
      else
      {
        size_t length = at->getNumElements();
        for (size_t i = 0; i < length; i++)
        {
          llvm::Value* elemPtr = env->builder->CreateConstGEP2_32(at, dest, 0, i);
          // We have a pointer to the element we are going to store into, but
          // because that element is itself an aggregate we call
          // StoreConstAggregate to perform the storing of that particular
          // element. because the src and dest have the same type, we know that 
          // the particular element we are considering storing into is the same 
          // type of constant element held within the constant array. Thus, 
          // we can pass in the llvm::Constant* to that element itself as the
          // source for the new call to StoreConstAggregate.
          StoreConstAggregate(at->getElementType(), elemPtr, src_array->getAggregateElement(i), env);
        }
      }
    }
    else if (llvm::StructType* st = llvm::dyn_cast<llvm::StructType>(ty))
    {
      llvm::ConstantStruct* cs = llvm::dyn_cast<llvm::ConstantStruct>(src);
      if (cs == nullptr)
        FatalError("Struct Type given to CreateStoreAggregate, but source is not a llvm::ConstantStruct", __FILE__, __LINE__);

      size_t i = 0;
      while (llvm::Constant* src_element = cs->getAggregateElement(i))
      {
        llvm::Value* dest_elem_ptr = env->builder->CreateConstGEP2_32(st, dest, 0, i);
        llvm::Type*  elem_type  = src_element->getType();
        
        if (elem_type->isSingleValueType())
        {
          env->builder->CreateStore(src_element, dest_elem_ptr);
        }
        else
        {
          StoreConstAggregate(elem_type, dest_elem_ptr, src_element, env);
        }

        i += 1;
      }

    }
    else 
    {
      FatalError("Unsupported Aggregate Type passed to StoreConstAggregate", __FILE__, __LINE__);
    } 
    // I am fairly sure this is valid semantics
    return dest;
  }

  llvm::Value* StoreValueAggregate(llvm::Type* ty, llvm::Value* dest, llvm::Value* src, std::shared_ptr<Environment> env)
  {
    if (llvm::ArrayType* at = llvm::dyn_cast<llvm::ArrayType>(ty))
    {
      llvm::Type* elem_ty = at->getElementType();

      if (elem_ty->isSingleValueType())
      {
        size_t length = at->getNumElements(), i = 0;
        while (i < length)
        {
          llvm::Value* dst_elem_ptr = env->builder->CreateConstGEP2_32(at, dest, 0, i);
          llvm::Value* src_elem_ptr = env->builder->CreateConstGEP2_32(at, src, 0, i);
          
          llvm::Value* src_value = env->builder->CreateLoad(elem_ty, src_elem_ptr);
          env->builder->CreateStore(src_value, dst_elem_ptr);

          i += 1;
        }
      }
      else
      {
        size_t length = at->getNumElements(), i = 0;
        while (i < length)
        {
          llvm::Value* dst_elem_ptr = env->builder->CreateConstGEP2_32(at, dest, 0, i);
          llvm::Value* src_elem_ptr = env->builder->CreateConstGEP2_32(at, src, 0, i);
          
          StoreValueAggregate(elem_ty, dst_elem_ptr, src_elem_ptr, env); 
          i += 1;
        }
      }
    }
    else if (llvm::StructType* st = llvm::dyn_cast<llvm::StructType>(ty))
    {
      size_t length = st->getNumElements(), i = 0;
      while (i < length)
      {
        llvm::Type*  elem_ty = st->getElementType(i);
        llvm::Value* dst_elem_ptr = env->builder->CreateConstGEP2_32(st, dest, 0, i);
        llvm::Value* src_elem_ptr = env->builder->CreateConstGEP2_32(st, src, 0, i);

        if (elem_ty->isSingleValueType())
        {
          llvm::Value* src_value = env->builder->CreateLoad(elem_ty, src_elem_ptr);
          env->builder->CreateStore(src_value, dst_elem_ptr);  
        }
        else 
        {
          StoreValueAggregate(elem_ty, dst_elem_ptr, src_elem_ptr, env);
        }

        i += 1;
      }
    }
    else
    {
      FatalError("Unsupported Aggregate Type passed to StoreValueAggregate", __FILE__, __LINE__);
    }

    return dest;
  }
}










/*-----------------*/
