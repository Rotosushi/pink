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
 *  for any nested depth of aggregate storage. given that the depth of recursion
 *  for a given aggregate type is rarely more than 5-10, and you have to loop
 *  through every single valued element in an iterative solution as well, the
 *  potential cost of this recursive solution is not too expensive in terms
 *  of space or time, compared to an imagined fully iterative solution.
 *
 *
 *
 *  there are two distinct cases that cannot be mixed however, and that is
 *  dependant upon the kind of source we are given, that is are we attempting
 *  to store a llvm::Value* or an llvm::Constant*. if we are storing the value
 *  from a llvm::Constant* then this procedure can be used for our
 *  constant initialization procedure. Now this is not useful for initializing
 *  GlobalVariables, as they can already be given aggregate llvm::Constant*
 *  as initializers, but it does come in handy when considering initializing
 *  a local variable with a constant initializer, or as in the other case
 *  when copying an existing aggregate to another memory location.
 *
 *
 *  instead of spliting this function in two internally with a huge
 *  if ... else ... clause, we are going to split the procedure into
 *  two procedures. one for storing a llvm::Constant* Aggregate into
 *  a memory location, and one for storing the contents of one memory
 *  location into another. Hold up, can't we support that with a memcpy?
 *  I think so, yeah, we just need the sizes of the types involved.
 *  however we cannot memcpy from a llvm::Constant, and the speedup of
 *  switching to memcpy really only shows when the types get large.
 *  so this procedure will still be useful for smallish types, so it
 *  will be nice to have around.
 *
 *  Input Constraints:
 *   -) type of src and dest must be equivalent, and each must have type
 *      '<ty>*' that is src and dest are pointers to type <ty>.
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
 *   -) env must be set up such that the instruction builder is in the correct
 *      position to emit the loads and stores for the values.
 *      this procedure will not modify the insertion position of the
 *      builder beyond the insertion of loads and stores.
 *
 *   Output constraints:
 *    -) none, this function returns void
 *
  #NOTE: #ASIDE:
  you could support named scoping, with symboltable lookup by simply always
  prepending the current scope name to each lookup. then following the exact
  same procedure as currently implemented, if that lookup fails we simply
  ask the scope above.
  as long as each variable is constructed by prepending the current scope name
  before binding that string to the symboltable, variable lookup would continue
  as usual. except that you would need a new kind of identifier from the lexer,
  which parsed fully qualified scope names, and then if the variable had a
  fully qualified scope name you would have to look for the symboltable with
  the corresponding scope name before lookup, and call a protected lookup
 procedure on the scope whos only difference is that you avoid the string
 concatenation of normal lookup. (and the problem of exctracting the last id
 from the scope)

  although we can lex fully qualified identifiers with a regular expression:
    id ('::' id)+


*/
void StoreAggregate(llvm::Type *type, llvm::Value *destination,
                    llvm::Value *source, const Environment &env) {
  assert(type != nullptr);
  assert(destination != nullptr);
  assert(source != nullptr);

  if (auto *const_src = llvm::dyn_cast<llvm::Constant>(source)) {
    StoreConstAggregate(type, destination, const_src, env);
  } else {
    StoreValueAggregate(type, destination, source, env);
  }
}

void StoreConstAggregate(llvm::Type *type, llvm::Value *destination,
                         llvm::Constant *source, const Environment &env) {
  assert(type != nullptr);
  assert(destination != nullptr);
  assert(source != nullptr);
  if (auto *array_type = llvm::dyn_cast<llvm::ArrayType>(type)) {

    //  so, if the element type of the array is a single value type,
    //  then we can safely use a llvm::StoreInstruction to perform the
    //  store. In the case where the element is itself an aggregate
    //  then for each element we want to store we recursively call this
    //  procedure to store it.
    if (array_type->getElementType()->isSingleValueType()) {
      size_t length = array_type->getNumElements();
      for (size_t i = 0; i < length; i++) {
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
        llvm::Value *elemPtr = env.instruction_builder->CreateConstGEP2_32(
            array_type, destination, 0, i);
        env.instruction_builder->CreateStore(source->getAggregateElement(i),
                                             elemPtr);
      }
    } else {
      size_t length = array_type->getNumElements();
      for (size_t i = 0; i < length; i++) {
        llvm::Value *elemPtr = env.instruction_builder->CreateConstGEP2_32(
            array_type, destination, 0, i);
        // We have a pointer to the element we are going to store into, but
        // because that element is itself an aggregate we call
        // StoreConstAggregate to perform the storing of that particular
        // element. because the src and dest have the same type, we know that
        // the particular element we are considering storing into is the same
        // type of constant element held within the constant array. Thus,
        // we can pass in the llvm::Constant* to that element itself as the
        // source for the new call to StoreConstAggregate.
        StoreConstAggregate(array_type->getElementType(), elemPtr,
                            source->getAggregateElement(i), env);
      }
    }
  } else if (auto *struct_type = llvm::dyn_cast<llvm::StructType>(type)) {
    // llvm::ConstantStruct* cs = llvm::dyn_cast<llvm::ConstantStruct>(src);
    // if (cs == nullptr)
    //   FatalError("Struct Type given to CreateStoreAggregate, but source is
    //   not a llvm::ConstantStruct", __FILE__, __LINE__);

    size_t idx = 0;
    while (llvm::Constant *src_element = source->getAggregateElement(idx)) {
      llvm::Value *dest_elem_ptr = env.instruction_builder->CreateConstGEP2_32(
          struct_type, destination, 0, idx);
      llvm::Type *elem_type = src_element->getType();

      if (elem_type->isSingleValueType()) {
        env.instruction_builder->CreateStore(src_element, dest_elem_ptr);
      } else {
        StoreConstAggregate(elem_type, dest_elem_ptr, src_element, env);
      }

      idx += 1;
    }

  } else {
    FatalError("Unsupported Aggregate Type passed to StoreConstAggregate",
               __FILE__, __LINE__);
  }
}

void StoreValueAggregate(llvm::Type *type, llvm::Value *destination,
                         llvm::Value *source, const Environment &env) {
  assert(type != nullptr);
  assert(destination != nullptr);
  assert(source != nullptr);
  if (auto *array_type = llvm::dyn_cast<llvm::ArrayType>(type)) {
    llvm::Type *elem_ty = array_type->getElementType();

    if (elem_ty->isSingleValueType()) {
      size_t length = array_type->getNumElements();
      size_t idx = 0;
      while (idx < length) {
        llvm::Value *dst_elem_ptr = env.instruction_builder->CreateConstGEP2_32(
            array_type, destination, 0, idx);
        llvm::Value *src_elem_ptr = env.instruction_builder->CreateConstGEP2_32(
            array_type, source, 0, idx);

        llvm::Value *src_value =
            env.instruction_builder->CreateLoad(elem_ty, src_elem_ptr);
        env.instruction_builder->CreateStore(src_value, dst_elem_ptr);

        idx += 1;
      }
    } else {
      size_t length = array_type->getNumElements();
      size_t idx = 0;
      while (idx < length) {
        llvm::Value *dst_elem_ptr = env.instruction_builder->CreateConstGEP2_32(
            array_type, destination, 0, idx);
        llvm::Value *src_elem_ptr = env.instruction_builder->CreateConstGEP2_32(
            array_type, source, 0, idx);

        StoreValueAggregate(elem_ty, dst_elem_ptr, src_elem_ptr, env);
        idx += 1;
      }
    }
  } else if (auto *struct_type = llvm::dyn_cast<llvm::StructType>(type)) {
    size_t length = struct_type->getNumElements();
    size_t idx = 0;
    while (idx < length) {
      llvm::Type *elem_ty = struct_type->getElementType(idx);
      llvm::Value *dst_elem_ptr = env.instruction_builder->CreateConstGEP2_32(
          struct_type, destination, 0, idx);
      llvm::Value *src_elem_ptr = env.instruction_builder->CreateConstGEP2_32(
          struct_type, source, 0, idx);

      if (elem_ty->isSingleValueType()) {
        llvm::Value *src_value =
            env.instruction_builder->CreateLoad(elem_ty, src_elem_ptr);
        env.instruction_builder->CreateStore(src_value, dst_elem_ptr);
      } else {
        StoreValueAggregate(elem_ty, dst_elem_ptr, src_elem_ptr, env);
      }

      idx += 1;
    }
  } else {
    FatalError("Unsupported Aggregate Type passed to StoreValueAggregate",
               __FILE__, __LINE__);
  }
}
} // namespace pink

/*-----------------*/
