#include "runtime/StaticCast.h"

#include "llvm/IR/DerivedTypes.h"

namespace pink {

// from the llvm perspective we have many different kinds of casting
// which can be done.
//
// for instance to cast between our boolean type and our integer type
// we have to emit a zext instruction
// and this would be the case for any smaller unsigned integer type, to
// any larger integer type (unsigned -> signed OR unsigned -> unsigned).
// if the smaller integer type is signed and the conversion is
// signed -> signed then we use the sext instruction.
// if the smaller integer type is signed and the conversion is
// signed -> unsigned then we can also use the zext instruction
// and then we have a choice, we could map mathematically to the
// unsinged integer and perform an abs instruction to convert
// signed to unsigned, then perform the zext OR we could leave the
// bits themselves alone when performing the cast, which may be useful
// from the perspective of some programmers. if we are doing low level
// code, then doing the least implicit bit manipulations will force the
// programmer to syntactically perform the bit manipulations
// themselves. which I think would lead to higher code legibility.
//
// or if we want to convert between our integer type and our boolean
// type we have to emit a trunc instruction.
// and this would be the case for any larger integer type to any
// smaller integer type. (signed -> signed OR unsigned -> unsigned)
// (the cross sign conversions can either be left alone, or we can
// consistently perform an abs instruction to map signed values to
// unsigned values. in the reverse case we must be aware that we
// can destroy data if the unsigned number being represented is a
// positive integer above the range of the signed integer.
//
//
// or if we want to convert between the different sizes of possible
// integers we can use zext to cast smaller to bigger and
// trunc to cast bigger to smaller
//
// or if we want to cast between our pointer types, we emit the
// bitcast instruction,
//
// of if we want to cast between addressspaces for a given pointer type
// we emit the addrspacecast instruction,
//
// or if we want to cast between an integer and a float we emit the
// sitofp instruction,
//
// or if we want to cast between a float and an integer we emit the
// fptosi instruction,
//
// or if we want to cast between a float and an unsigned integer we
// emit the fptoui instruction,
//
// or if we want to cast between an unsigned integer and a float we
// emit the uitofp instruction
//
//
auto StaticCast(llvm::Value *value, llvm::Type *target_type, Environment &env)
    -> Outcome<llvm::Value *, Error> {
  llvm::Type *value_type = value->getType();

  // so based on the to_type and the from_type we need to choose our cast
  // instruction. to tell what the to type is exactly we can use either
  // llvm::isa or llvm::dyn_cast, however since we probably want to use the
  // information held within the to type, this means we want dyn_cast.
  //
  // there are a few main types which we want to be able to cast between,
  // integers
  // pointers
  // floating point types
  //
  //
  if (auto *from_type = llvm::dyn_cast<llvm::IntegerType>(value_type)) {
    // so there are a few main cast operations on integer types
    // an int type cast to another int type
    // an int type cast to a float type
    // an int type cast to a pointer type
    //
    if (auto *to_type = llvm::dyn_cast<llvm::IntegerType>(target_type)) {
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
      if (from_bitwidth < to_bitwidth) {
        return {
            env.instruction_builder->CreateZExt(value, target_type, "zext")};
      }

      if (from_bitwidth > to_bitwidth) {
        return {
            env.instruction_builder->CreateTrunc(value, target_type, "trunc")};
      }

      // from_bitwidth == to_bitwidth
      return {env.instruction_builder->CreateBitCast(value,
                                                     target_type,
                                                     "bitcast")};
    }
    // else if (llvm::PointerType* to_type =
    // llvm::dyn_cast<llvm::PointerType>(target_type)) else if
    // (to_type->isFloatingPointTy());
    return {Error(Error::Code::CannotCastToType, Location())};
  }
  // else if (llvm::PointerType* from_type =
  // llvm::dyn_cast<llvm::PointerType>(target_type)) else if
  // (from_type->isFloatingPointTy())
  return {Error(Error::Code::CannotCastFromType, Location())};
}

} // namespace pink
