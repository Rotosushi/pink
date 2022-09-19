/**
 * @file TupleType.h
 * @brief Header for class TupleType
 * @version 0.1
 */
#pragma once 
#include <vector>

#include "type/Type.h"

namespace pink {
  /**
   * @brief Represents the Type of a Tuple
   */
  class TupleType : public Type {
  public:
    /**
     * @brief The Types of the Tuple members
     */
    std::vector<Type*> member_types;

    /**
     * @brief Construct a new TupleType
     * 
     * @param member_types the member Types of this TupleType
     */
    TupleType(std::vector<Type*> member_types);

    /**
     * @brief Destroy the Tuple Type
     */
    virtual ~TupleType();

    /**
     * @brief Implements LLVM style [RTTI] for this class
     * 
     * @param type the type being tested
     * @return true if type *is* an instance of TupleType
     * @return false if type *is not* an instance of TupleType
     */
    static bool classof(const Type* type);

    /**
     * @brief Computes if other is equivalent to this TupleType
     * 
     * @param other the other type
     * @return true if other *is* equivalent to this TupleType
     * @return false if other *is not* equivalent to this TupleType
     */
    virtual bool EqualTo(Type* other) override;

    /**
     * @brief Compute the cannonical string representation of this TupleType
     * 
     * @return std::string the string representation
     */
    virtual std::string ToString() override;

    /**
     * @brief Compute the llvm::Type equivalent to this TupleType
     * 
     * @param env the environment of this compilation unit
     * @return Outcome<llvm::Type*, Error> if true the llvm::Type equivalent to this TupleType,
     * if false the Error encountered
     */
    virtual Outcome<llvm::Type*, Error> Codegen(const Environment& env) override;
  };
}
