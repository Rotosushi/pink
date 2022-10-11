
#include "type/FunctionType.h"

#include "aux/Environment.h"

namespace pink {
FunctionType::FunctionType(Type *return_type,
                           const std::vector<Type *> &args_types)
    : Type(Type::Kind::Function), result(return_type), arguments(args_types) {}

auto FunctionType::classof(const Type *type) -> bool {
  return type->GetKind() == Type::Kind::Function;
}

auto FunctionType::EqualTo(Type *other) const -> bool {
  bool equal = true;

  // the function types are equal if they have the same
  // return type, and the same number of argument types,
  // and each argument at each position is identical in type.
  auto *oft = llvm::dyn_cast<FunctionType>(other);
  if (oft != nullptr) {
    if (oft->result != result) {
      equal = false;
    } else {
      auto len = arguments.size();
      auto olen = oft->arguments.size();

      if (len != olen) {
        equal = false;
      } else {
        for (size_t i = 0; i < len; i++) {
          if (arguments[i] != oft->arguments[i]) {
            equal = false;
            break;
          }
        }
        // the types are equal if the evaluation reaches here
      }
    }
  } else {
    equal = false;
  }

  return equal;
}

auto FunctionType::ToString() const -> std::string {
  std::string str;
  if (!arguments.empty()) {
    for (auto *type : arguments) {
      str += type->ToString() + " -> ";
    }
  } else {
    str += "Void -> ";
  }

  str += result->ToString();
  return str;
}

/*
 *  So we know that the actual function type that a given function has is
 *  in some way dependant upon the types within the function type itself.
 *  specifically, any argument which is not a single value type must be
 *  promoted to a pointer to the same type, and that argument must be given
 *  the parameter attribute byval(<ty>).
 *  Then, if the return type of the function is similarly not a single
 *  value type, we must set the llvm::FunctionType's return type to void,
 *  and add a parameter with the attribute sret(<ty>).
 *
 *  There is however, a slight complication with this however. namely that
 *  we cannot add parameter attributes to llvm::FunctionTypes, we instead
 *  add them to the llvm::Function. So, within this procdure, we simply
 *  need to promote structure type parameters to pointers to the same
 *  structure type, and modify the return type if the return value is a
 *  structure type too.
 *
 *  I am a little worried that we are destroying information here, because
 *  if we promote the argument to a pointer, than how will
 *  Function::Codegen know to place the byval(<ty>) parameter on that
 *  specific function argument? consider the case where a function takes
 *  by definition a structure and a pointer to a structure, after
 *  FunctionType::Codegen now both parameters will be pointers to
 *  structures, so how does Function::Codegen know which one is the
 *  byval(<ty>)? Actually I think we could reference the pink::FunctionType
 *  to answer that question.
 *
 */
auto FunctionType::Codegen(const Environment &env) const
    -> Outcome<llvm::Type *, Error> {
  std::vector<llvm::Type *> llvm_args(arguments.size());

  for (auto *type : arguments) {
    Outcome<llvm::Type *, Error> arg_res = type->Codegen(env);

    if (!arg_res) {
      return arg_res;
    }

    // if the llvm::Type is not a single value type,
    // promote the type to a pointer type
    if (arg_res.GetFirst()->isSingleValueType()) {
      llvm_args.emplace_back(arg_res.GetFirst());
    } else {
      // #NOTE: We know that this argument is going to be passed
      // byvalue to the function, and that llvm is going to
      // perform the copy of the parameters memory. presumably
      // that allocation and subsequent copy must happen in local
      // memory, So i think the address space for this pointer
      // is the Alloca address space.
      llvm_args.emplace_back(env.instruction_builder->getPtrTy(
          env.data_layout.getAllocaAddrSpace()));
    }
  }

  Outcome<llvm::Type *, Error> res_res = result->Codegen(env);

  if (!res_res) {
    return res_res;
  }

  if (!llvm_args.empty()) {
    if (res_res.GetFirst()->isSingleValueType() ||
        res_res.GetFirst()->isVoidTy()) {
      llvm::Type *fn_ty = llvm::FunctionType::get(res_res.GetFirst(), llvm_args,
                                                  /* isVarArg */ false);
      return {fn_ty};
    }
    // promote return value to an argument as a pointer
    llvm_args.insert(llvm_args.begin(),
                     env.instruction_builder->getPtrTy(
                         env.data_layout.getAllocaAddrSpace()));
    llvm::Type *fn_ty = llvm::FunctionType::get(
        env.instruction_builder->getVoidTy(), llvm_args, /* isVararg */ false);
    return {fn_ty};
  }

  llvm::Type *res_ty = res_res.GetFirst();
  if (res_ty->isSingleValueType() || res_ty->isVoidTy()) {
    llvm::Type *fn_ty =
        llvm::FunctionType::get(res_res.GetFirst(), /* isVararg */ false);
    return {fn_ty};
  }

  // promote the return value to an argument as a pointer
  llvm_args.insert(
      llvm_args.begin(),
      env.instruction_builder->getPtrTy(env.data_layout.getAllocaAddrSpace()));
  llvm::Type *fn_ty = llvm::FunctionType::get(
      env.instruction_builder->getVoidTy(), llvm_args, /* isVarArg */ false);
  return {fn_ty};
}
} // namespace pink
