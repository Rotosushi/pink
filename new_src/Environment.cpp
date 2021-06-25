#include "llvm/Support/Casting.h"

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instruction.h"

#include "Environment.hpp"

namespace pink {

Environment::Environment(const Parser&               parser,
                         const StringInterner&       interned_names,
                         const StringInterner&       interned_operators,
                         const SymbolTable&          symbols,
                         const BinopTable&           binops,
                         const BinopPrecedenceTable& bpt,
                         const UnopTable&            unops,
                         const std::string&          target_triple,
                         const llvm::DataLayout&     data_layout,
                         const llvm::LLVMContext&    context,
                         const llvm::Module&         module,
                         const llvm::IRBuilder&      builder);
  : parser(parser),               interned_names(interned_names), interned_operators(interned_operators),
    symbols(symbols),             binops(binops),                 binop_precedence_table(bpt),
    unops(unops),                 target_triple(target_triple),   data_layout(data_layout),
    context(context),             module(module),                 builder(builder)
{

}


// sooo, yeah. technically/semantically nil -is- false.
// even though conceptually they have distinct types.
llvm::IntegerType* GetNilType()
{
  return llvm::Type::getInt1Ty(context);
}

llvm::IntegerType* GetBooleanType()
{
  return llvm::Type::getInt1Ty(context);
}

llvm::IntegerType* GetIntegerType()
{
  return llvm::Type::getInt64Ty(context);
}

/*
  https://llvm.org/doxygen/classllvm_1_1IntegerType.html
  ~> ...
    """MAX_INT_BITS | maximum number of bits that can be specified
                    Note that bit width is stored in the Type classes SubclassData
                    field which has 24 bits. This yields a maximum bit width of
                    16,777,215 bits"""
    ...
  this is a weird thought, but any structure that
  is smaller than 16 million some odd bits can be
  encoded as a single llvm::IntegerType holding the
  same exact bitpattern. could we simply reinterpret_cast
  the Type to an IntegerType? to treat it as a bitpattern
  instead of a typed structure? we could also imagine a
  converse operation. the only issue is the semantics of
  construction, especially with complex objects which require
  specific initialization patterns.
  if we are imagining
  a flattening to binary to transmit peices of the program around
  then this operation could be well-formed, as long as we are
  only building objects from bitpatterns that were built
  from the same type of object. which is something that
  could be negotiated by the language.
  this is dangerous in the general use case, and so should be
  guarded in a modern design. but if we were aiming for
  usable immediately we could provide a small barebones
  unsafe implementation, essentaially just the basic primitive
  operations, and eventually build up a library around them
  to be exported as part of the standard.
*/


/*
  given a primitive type, builds a object that is the
  right type, default initialized. this is uniformly
  going to be zero, unless specifically required by the
  type itself.
  which is not the case for most primitive types.
  standard i/o and thread objects are the only things i
  can think where we don't want default initialzed
  objects that are simply zeroed out.
*/
Judgement Environment::BuildZeroedValue(llvm::Type* t)
{
  if (llvm::IntegerType* intTy = llvm::dyn_cast<llvm::IntegerType>(t))
  {
    // currently, all three of our types are integer types
    // this is because integers can be used to encode all three finite types.
    // and llvm provides us with a handy method within
    // this circumstance.
    // get's a llvm::ConstantInt of the specified type and value.
    // we are garunteed to be able to store the value 0 in any
    // size ConstantInt, and thus we can default initialize
    // any ConstantInt type safely with this statement.
    // https://llvm.org/doxygen/classllvm_1_1ConstantInt.html
    return llvm::ConstantInt::get(intTy, 0);
  }
  else
  {
    // this is a small kindness to debugging this statement.
    std::string buf;
    llvm::raw_string_ostream hdl(buf);
    type->print(hdl);

    std::string errtxt = hdl.str();

    FatalError("Unsupported type for default initialization ~> " + errtxt, __FILE__, __LINE__);
  }
}



/*
  all local bindings get allocated in the beginning
  of the function, this is the requirement for local
  binding optimizations from LLVM.
  to make this convenient we require a reference to the
  procedure directly, this delegates getting this reference
  to the procedure to the caller, which means we can handle
  it hopefully a little easier, as each place that could
  possibly want to allocate a local should have a direct
  reference to a current InsertionPoint (llvm::BasicBlock)
  which has a reference to it's containing function.

  https://llvm.org/docs/LangRef.html#alloca-instruction
  https://llvm.org/doxygen/classllvm_1_1AllocaInst.html
  https://llvm.org/doxygen/classllvm_1_1PointerType.html
*/
llvm::AllocaInst* Environment::BuildLocalAlloca(llvm::Type* type, std::string& name, llvm::Function* fn)
{
  if (!value || !fn)
    return nullptr;

  // to satisfy the 'insert all alloca's at the beginning
  // of the functions InitialBlock' constraint, we get the
  // Function's EntryBlock's FirstInsertionPoint.
  // this should have the aggregate effect of 'prepending'
  // all allocations within the entryblock. in
  // effect, allocating each variable declared
  // one position lower than the next variable
  // declaration.
  // (i.e. it reverses their allocation order)
  llvm::BasicBlock*          insertion_block = &(fn->GetEntryBlock());
  llvm::BasicBlock::iterator insertion_point = insertion_block->GetFirstInsertionPt();

  // build a temporary local builder to perform the insertion
  // so we do not have to save/restore the insertion point of
  // the IRBuilder we have as a member.
  llvm::IRBuilder local_builder(insertion_block, insertion_point);

  llvm::AllocaInst* local_storage  = local_builder.CreateAlloca(type,
                                                                data_layout.getAllocaAddrSpace(),
                                                                nullptr, /* array size, used for alloca'ing arrays */
                                                                name);
  return local_storage;
}


/*
  BuildLoad emits one or a series of IRBUilder instructions,
  where we load an alloca into a constant of the same type
  and value. essentially implementing the unwrap function
  of a pointer, as one thinks of it in C.

  the assumption of this procedure is that every
  primitive type of the language isSingleValueType
  (we can store it in a machine register on the target)
  and that any element that is larger than a single
  register is that way because it is an aggregate type.
  thus we can write a simple recursive procedure to load
  each element that is a register size, and recurring
  to emit the right series of loads to get the next

  https://llvm.org/docs/LangRef.html#load-instruction
*/
llvm::Value* Environment::BuildLoad(llvm::AllocaInst* alloc)
{
  if (!alloc)
    return nullptr;

  llvm::Type* allocated_type = alloc->getAllocatedType();

  // so, i'm not 100% on this, but by the definition
  // given by llvm, it would be reasonable to assume
  // that a struct type whose AllocaSize was smaller
  // than a single word would be a SingleValueType.
  // otherwise this would be using the DataLayout to
  // calculate the AllocaSize and then comparing that
  // to the maximum register value. this is what
  // I assume defined the mapping of types to booleans
  // that i am sure this function is implemented as.
  if (allocated_type->isSingleValueType())
  {
    return builder.CreateAlignedLoad(allocated_type,
                                     alloc,
                                     alloc->getAlign());
  }
  else
  {
    llvm::Value* agg_ptr = llvm::dyn_cast<llvm::Value>(alloc);

    if (!agg_ptr)
      FatalError("We couldn't cast the Allocation to a Pointer!", __FILE__, __LINE__);

    return BuildLoadAggregate(agg_ptr, agg_ptr->getType());
  }
}

/*
  this algorithm is O(N) for arrays and structs of single cell types.
  the use of recursion makes it O(N^M) where M is the depth of the
  recursion of the type itself.
*/
llvm::Value* Environment::BuildLoadAggregate(llvm::Value* value_ptr, llvm::Type* pointee_type)
{

  if (llvm::StructType* struct_type = llvm::dyn_cast<llvm::StructType>(pointee_type))
  {
    // so we need to build a new structure in memory to
    // return as a value. (i assume) so, this means loading each field
    // and then building a ConstantStruct out of each of
    // these loads. if the submember of the struct
    // is itself too big to fit in a single load,
    // we recurse, which will handle the case where
    // we have a struct made of structs.
    llvm::ArrayRef<Type*>     struct_element_types = struct_type->elements();
    std::vector<llvm::Constant*> new_struct_members;

    for (std::size_t i = 0; i < struct_element_types.size(); i++)
    {
      // the builder expects the index in the form of an llvm::Value*
      // for this particular instruction.
      llvm::Value* elem_index  = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), i);

      llvm::Value* loaded_elem = nullptr;
      llvm::Type*  elem_type   = struct_element_types[i];

      // get element pointer computes the address of the
      // element of aggregate types. how does it do this?
      //  -- https://llvm.org/docs/GetElementPtr.html --
      // it takes a pointer to some type. the first index treats
      // the pointer as if it pointed to an array of that type
      // from the initial point. so as if in C you had typed:
      // Type* Foo = ...;
      // &(Foo[idx0])
      // and the second index indexes the structure at that point
      // in memory.
      // &(Foo[idx0].idx1)
      // so, because we have been given a single Alloca, which is a Ptr to our Type
      // we want to always index the first array cell of that pointer, which
      // will be the structure itself/the alloca space itself,
      // and we index element i. which is
      // the element this run of the loop is working on.
      /*llvm::Value* elem_ptr = builder.CreateConstInBoundsGEP2_32(struct_type,
                                                                 value_ptr,
                                                                 0,
                                                                 i);
      we are using this one instruction, because it seems to replace
      these two bit's of code entirely.
      */
      if (elem_type->isSingleValueType())
      {
        loaded_elem = builder.CreateExtractElement(value_ptr, elem_index);
      }
      else
      {
        loaded_elem = BuildLoadAggregate(elem_ptr, elem_type);
      }

      llvm::Constant* new_elem = llvm::dyn_cast<llvm::Constant*>(loaded_elem);

      if (!new_elem)
        FatalError("aggregate member must be a constant", __FILE__, __LINE__);

      new_struct_members += new_elem;
    }

    return llvm::ConstantStruct::get(struct_type, new_struct_members);
  }
  else if (llvm::ArrayType* array_type = llvm::dyn_cast<llvm::ArrayType>(pointee_type))
  {
    llvm::Type* elem_type       = array_type->getElementType();
    std::size_t num_array_elem  = array_type->getNumElements();
    llvm::Align elem_alignment  = data_layout.getABITypeAlign(elem_type);
    std::vector<llvm::Constant*> new_array_elements;

    if (elem_type->isSingleValueType())
    {
      for (std::size_t i = 0; i < num_array_elem; i++)
      {
        llvm::Value* elem_ptr = builder.CreateConstInBoundsGEP2_32(array_type,
                                                                value_ptr,
                                                                0,
                                                                i);

        llvm::LoadInst* load = builder.CreateAlignedLoad(elem_type, elem_ptr, elem_alignment);

        llvm::Constant* loaded_elem = llvm::cast<llvm::Constant*>(load);
        new_array_elements += loaded_elem;
      }
    }
    else
    {
      for (std::size_t i = 0; i < num_array_elem; i++)
      {
        llvm::Value* elem_ptr = builder.CreateConstInBoundsGEP2_32(array_type,
                                                                value_ptr,
                                                                0,
                                                                i);

        llvm::Value* loaded_elem = BuildLoadAggregate(elem_ptr, elem_type);

        llvm::Constant* new_elem = llvm::dyn_cast<llvm::Constant*>(loaded_elem);

        if (!new_elem)
          FatalError("aggregate member must be a constant", __FILE__, __LINE__);

        new_array_elements += new_elem;
      }
    }

    return llvm::ConstantArray::get(array_type, new_array_elements);
  }
  else
  {
    FatalError("unsupported aggregate type for loading", __FILE__, __LINE__);
  }
}

/*
  takes an alloca which has already been built to hold the constant
  being stored, and the constant being stored.
  this function handles the case where the value we want to store
  is too big to hold within a single register. which means we need
  to use multiple stores instead.

  https://llvm.org/docs/LangRef.html#store-instruction
*/
void BuildStore(llvm::AllocaInst* pointer, llvm::Constant* value)
{
  llvm::Type* value_type     = value->getType();
  llvm::Type* alloca_type    = pointer->getAllocatedType();
  llvm::TypeSize value_size  = data_layout.getTypeAllocSize(value_type);
  llvm::TypeSize alloca_size = data_layout.getTypeAllocSize(alloca_type);

  if (value_size <= alloca_size)
  {
    // if the value can fit in a single register
    // then we can use a single LLVM store instruction
    if (value_type->isSingleValueType())
    {
      builder.CreateAlignedStore(value, pointer, pointer->getAlign());
    }
    else
    {
      BuildStoreAggregate(pointer, value);
    }
  }
  else
  {
    std::string buf, errtxt;
    llvm::raw_string_ostream hdl(buf);
    errtxt = hdl.str();
    FatalError("Allocation not big enough to hold Type. ~> " + errtxt, __FILE__, __LINE__);
  }
}


void BuildStoreAggregate(llvm::Value* pointer, llvm::Constant* value)
{
  // elements are for arrays, members are for structs.
  // they are both memory cells with type.
  llvm::Type* value_type = value->getType();

  if (llvm::StructType* struct_type = llvm::cast<llvm::StructType>(value_type))
  {
    // build a GEP to each pair of member in the alloca space,
    // and within the value literal, then attempt to store the value
    // from the literal into the alloca

    llvm::ArrayRef<llvm::Type*> member_types = struct_type->elements();

    // if the struct member is itself bigger than a single value type,
    // we assume it is because it is another aggregate type.

    for (std::size_t i = 0; i < member_types.size(); i++)
    {
      llvm::Type* member_type  = member_types[i];
      llvm::Align member_align = data_layout.getABITypeAlign(member_type);

      if (member_type->isSingleValueType())
      {
        /*
        // get a pointer to the member whose type we have (member [i])
        llvm::Value* value_member_ptr = builder.CreateConstInBoundsGEP2_32(struct_type, // getting a pointer to this type
                                                                           value, // this is what we get a pointer to
                                                                           0,     // we get a pointer to element 0
                                                                           i);    // we get a pointer to member i
        // get a pointer to the same member in the alloca space.
        llvm::Value* alloca_member_ptr = builder.CreateConstInBoundsGEP2_32(struct_type, pointer, 0, i);
        // we can build a single store instruction here.
        llvm::StoreInst* stored_member = builder.CreateAlignedStore(value_member_ptr, alloca_member_ptr, member_align);
        */

        llvm::Value* value_element = builder.CreateExtractElement(value, i);
        llvm::Value* inserted_elem = builder.CreateInsertElement(pointer, value_element, i);
      }
      else // load an aggregate member
      {
        // get a pointer to the member within the value
        llvm::Value* value_member_ptr = builder.CreateConstInBoundsGEP2_32(struct_type, value, 0, i);
        // get a pointer to the member within the alloca
        llvm::Value* alloca_member_ptr = builder.CreateConstInBoundsGEP2_32(struct_type, pointer, 0, i);
        // store the aggregate member
        BuildStoreAggregate(alloca_member_ptr, value_member_ptr);
      }
    }

  }
  else if (value_type->isArrayTy())
  {
    llvm::ArrayType* array_type = llvm::cast<llvm::ArrayType*>(value_type);

    llvm::Type* element_type  = array_type->getElementType();
    llvm::Align element_align = data_layout.getABITypeAlign(element_type);

    if (element_type->isSingleValueType())
    {
      std::size_t array_length = array_type->getNumElements();
      for (std::size_t i = 0; i < array_length; i++)
      {
        // alloca[i] = value[i]
        // get a pointer to the element within the constant
        llvm::Value* value_element_ptr  = builder.CreateConstInBoundsGEP2_32(array_type, value, 0, i);
        // get a pointer to the element within the alloca
        llvm::Value* alloca_element_ptr = builder.CreateConstInBoundsGEP2_32(array_type, pointer, 0, i);
        // store the single value element
        llvm::StoreInst* stored_element = builder.CreateAlignedStore(value_element_ptr, alloca_element_ptr, element_align);
      }
    }
    else // every element is bigger than a single word.
    {
      std::size_t array_length = array_type->getNumElements();
      for (std::size_t i = 0; i < array_length; i++)
      {
        llvm::Value* value_element_ptr = builder.CreateConstInBoundsGEP2_32(array_type, value, 0, i);
        llvm::Value* alloca_element_ptr = builder.CreateConstInBoundsGEP2_32(array_type, pointer, 0, i);
        BuildStoreAggregate(alloca_element_ptr, value_element_ptr);
      }
    }
  }
  else
  {
    FatalError("Unsupported aggregate type for storing.", __FILE__, __LINE__);
  }
}


}








// --
