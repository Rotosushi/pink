/**
 * @file ArrayType.h
 * @brief Header for class ArrayType
 * @version 0.1
 */
#pragma once 
#include "type/Type.h"

namespace pink {
  /**
   * @brief Represents the Type of an Array
   * 
   */
  class ArrayType : public Type {
  public:
    /**
     * @brief The size of the Array
     * 
     */
    size_t size;

    /**
     * @brief The member type of the Array
     * 
     */
    Type*  member_type;

    /**
     * @brief Construct a new ArrayType 
     * 
     * @param size the size of the ArrayType
     * @param member_type the member Type of the ArrayType
     */
    ArrayType(size_t size, Type* member_type);

    /**
     * @brief Destroy the ArrayType
     * 
     */
    ~ArrayType();

    /**
     * @brief Implements LLVM style [RTTI] for this class
     * 
     * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
     * 
     * @param type the type to test
     * @return true if type *is* an ArrayType
     * @return false if type *is not* an ArrayType
     */
    static bool classof(const Type* type);

    /**
     * @brief Compute if this ArrayType is equivalent to the 'other' Type
     * 
     * @param other the other Type
     * @return true if other *is* equivalent to this ArrayType
     * @return false if other *is not* equivalent to this ArrayType
     */
    virtual bool EqualTo(Type* other) override;

    /**
     * @brief Compute the cannonical string representation of this ArrayType
     * 
     * @return std::string the string representation
     */
    virtual std::string ToString() override;

    /**
     * @brief Compute the llvm equivalent of this ArrayType
     * 
     * @param env the environment of this compilation unit
     * @return Outcome<llvm::Type*, Error> if true then the llvm::Type of this ArrayType,
     * if false then the Error encountered
     */
    virtual Outcome<llvm::Type*, Error> Codegen(const Environment& env) override;
  };
}

