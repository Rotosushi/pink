




namespace pink {

  /*
   *  this proceudre is meant to be able to construct the proper 
   *  number and sequence of store instructions such that we store 
   *  the data from source to dest.
   *
   *  this procedure is here because the store instruction can only 
   *  support storing single value types. That is, types that can fit
   *  within a single register from codegeneration's perspective.
   *
   *
   *
   *
   *
   */
  void CreateAggregateStore(llvm::Value* destination, llvm::Value* source, std::shared_ptr<Environment> env)
  {
    // there are a few cases here, 
    // 1) storing an array or struct where each member is of single value type
    // 2) storing an array or struct where some member is of aggregate value type
    // 3) storing an array of structs where each member of the struct is
    //    a single value type
    // 4) storing an array of structs where some member of the struct is 
    //    an aggregate value type.
    //
    // so, looking at this, it seems like we can break these into two, by
    // considering either storing an array type, or storing a struct type.
    //
    llvm::Type* destTy = destination->getType();
    llvm::Type* srcTy  = source->getType();
    
    if (destTy != srcTy)
    {
      FatalError("Source and Destination Types are not equal, cannot store aggregate type into non-matching type", __FILE__, __LINE__);
    } 

    // we also have a distinction to make where the source could be a Constant
    // or not. if the source is Constant we have to access it's members
    // differently. however in that case, we would expect that if a member of
    // the constant value was itself an aggregate, then the constant aggregates 
    // members will also be constant.
  }

}
