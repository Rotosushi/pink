#include "llvm/Support/Casting.h"

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instruction.h"

#include "Environment.hpp"


Environment::Environment (std::shared_ptr<StringInterner>    interned_names,
                          std::shared_ptr<StringInterner>    interned_operators,
                          std::shared_ptr<SymbolTable>       symbols,
                          std::shared_ptr<BinopSet>          binops,
                          std::shared_ptr<UnopSet>           unops,
                          llvm::StringRef                    target_triple,
                          std::shared_ptr<llvm::DataLayout>  data_layout,
                          std::shared_ptr<llvm::LLVMContext> context,
                          std::shared_ptr<llvm::Module>      module,
                          std::shared_ptr<llvm::IRBuilder>   builder);
  : interned_names(interned_names), interned_operators(interned_operators),
    symbols(symbols),   binops(binops), unops(unops),   target_triple(target_triple),
    data_layout(data_layout), context(context), module(module), builder(builder)
{

}

// these names could honestly just be public,
// and it would be identical.
const StringInterner& Environment::GetInternedNames()
{
  return interned_names;
}

const StringInterner& Environment::GetInternedOperators()
{
  return interned_operators;
}

const SymbolTable& Environment::GetSymbolTable()
{
  return symbols;
}

const BinopSet& Environment::GetBinopTable()
{
  return binops;
}

const UnopSet& Environment::GetUnopTable()
{
  return unops;
}

const std::string& Environment::GetTargetTriple()
{
  return target_triple;
}

const llvm::DataLayout& Environment::GetDataLayout()
{
  return data_layout;
}

const llvm::DataLayout& Environment::GetContext()
{
  return context;
}

const llvm::Module& Environment::GetModule()
{
  return module;
}

const llvm::IRBuilder& Environment::GetIRBuilder()
{
  return builder;
}

/*
  all local bindings get allocated in the beginning
  of the function, this is the requirement for local
  binding optimizations from LLVM.
*/
llvm::AllocaInst* Environment::BuildLocalAlloca(llvm::Type* type, std::string& name, llvm::Function* fn)
{
  if (!value || !fn)
    return nullptr;

  // to satisfy the 'insert all alloca's at the beginning
  // of the functions InitialBlock' constraint, we get the
  // Function's EntryBlock's FirstInsertionPoint.
  // this should have the aggregate effect or 'prepending'
  // all allocations within the entryblock. in
  // effect, allocating each variable declared
  // one position lower than the next variable
  // declaration.
  // (i.e. it reverses their allocation order)
  llvm::BasicBlock*          insertion_block = &(fn->GetEntryBlock());
  llvm::BasicBlock::iterator insertion_point = insertion_block->GetFirstInsertionPt();

  llvm::IRBuilder local_builder(insertion_block, insertion_point);

  llvm::AllocaInst* local_storage  = local_builder.CreateAlloca(type,
                                                                data_layout.getAllocaAddrSpace(),
                                                                nullptr, /* array size */
                                                                name);
  return local_storage;
}


/*
  BuildLoad is a collection of IRBUilder instructions,
  where we load an alloca into a constant of the same type
  and value. essentially implementing the unwrap function
  of a pointer, as one thinks of it in C.

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
  if (allocated_type->isSingleValueType())
  {
    return builder.CreateAlignedLoad(allocated_type,
                                     alloc,
                                     alloc->getAlign());
  }
  else
  {
    llvm::Constant* agg_ptr = llvm::dyn_cast<llvm::Constant*>(alloc);

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

  if (pointee_type->isStructTy())
  {
    // so we need to build a new structure in memory to
    // return as a value. (i assume) so, this means loading each field
    // and then building a ConstantStruct out of each of
    // these loads. if the submember of the struct
    // is itself too big to fit in a single load,
    // we recurse, which will handle the case where
    // we have a struct made of structs.
    llvm::StructType*         struct_type    = llvm::cast<llvm::StructType*>(pointee_type);
    llvm::ArrayRef<Type*>     struct_element_types = struct_type->elements();
    std::vector<llvm::Constant*> new_struct_members;

    for (std::size_t i = 0; i < struct_element_types.size(); i++)
    {
      llvm::Value* loaded_elem = nullptr;
      llvm::Type*  elem_type   = struct_element_types[i];

      // get element pointer computes the address of the
      // element of aggregate types. how does it do this?
      // it takes a pointer to some type. the first index treats
      // the pointer as if it pointed to an array of that type
      // from the point. so as if in C you had typed:
      // Type* Foo = ...;
      // Foo[idx0]
      // and the second index indexes the structure at that point
      // in memory.
      // &(Foo[idx0].idx1)
      // so, because we have been given a single Alloca, which is a Value Ptr
      // we want to always index the first element of the pointer, which
      // will be the structure itself, and we index element i. which is
      // the element this run of the loop is working on.
      llvm::Value* elem_ptr = builder.CreateConstInBoundsGEP2_32(struct_type,
                                                              value_ptr,
                                                              0,
                                                              i);

      if (elem_type->isSingleValueType())
      {
        llvm::Align elem_align = data_layout.getABITypeAlign(elem_type);

        // llvm supports efficient loads of first class types.
        loaded_elem = builder.CreateAlignedLoad(elem_type, elem_ptr, elem_align);
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
  else if (pointee_type->isArrayTy())
  {
    llvm::ArrayType* array_type = llvm::cast<llvm::ArrayType*>(pointee_type);
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
  is too big to hold within a single register. which means we ned
  to use multiple stores instead.
*/
void BuildStore(llvm::AllocaInst* pointer, llvm::Constant* value)
{
  llvm::Type* value_type     = value->getType();
  llvm::Type* alloca_type    = pointer->getAllocatedType();
  llvm::TypeSize value_size  = data_layout.getTypeAllocSize(value_type);
  llvm::TypeSize alloca_size = data_layout.getTypeAllocaSize(alloca_type);

  if (value_size <= alloca_size)
  {
    // if the value can fit in a single register
    // then we can use a single LLVM store instruction
    if (value_size->isSingleValueType())
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
    FatalError("Allocation not big enough to hold Constant.", __FILE__, __LINE__);
  }
}


void BuildStoreAggregate(llvm::Value* pointer, llvm::Constant* value)
{
  // elements are for arrays, members are for structs.
  // they are both memory cells with type.
  llvm::Type* value_type = value->getType();

  if (value_type->isStructTy())
  {
    // build a GEP to each pair of elements in the alloca space,
    // and within the value literal, then attempt to store the value
    // from the literal into the alloca
    llvm::StructType* struct_type = llvm::cast<llvm::StructType*>(value_type);

    llvm::ArrayRef<llvm::Type*> member_types = struct_type->elements();

    // if the struct member is itself bigger than a single value type,
    // we assume it is because it is another aggregate type.

    for (std::size_t i = 0; i < member_types.size(); i++)
    {
      llvm::Type* member_type  = member_types[i];
      llvm::Align member_align = data_layout.getABITypeAlign(member_type);

      if (member_type->isSingleValueType())
      {
        // get a pointer to the member whose type we have (member [i])
        llvm::Value* value_member_ptr = builder.CreateConstInBoundsGEP2_32(struct_type, // getting a pointer to this type
                                                                           value, // this is what we get a pointer to
                                                                           0,     // we get a pointer to element 0
                                                                           i);    // we get a pointer to member i
        // get a pointer to the same member in the alloca space.
        llvm::Value* alloca_member_ptr = builder.CreateConstInBoundsGEP2_32(struct_type, pointer, 0, i);
        // we can build a single store instruction here.
        llvm::StoreInst* stored_member = builder.CreateAlignedStore(value_member_ptr, alloca_member_ptr, member_align);
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











// --
