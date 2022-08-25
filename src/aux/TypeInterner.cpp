
#include "aux/TypeInterner.h"

namespace pink {
    TypeInterner::TypeInterner()
        : nil_type(nullptr), bool_type(nullptr), int_type(nullptr)
    {

    }

    TypeInterner::~TypeInterner()
    {

    }

    NilType* TypeInterner::GetNilType()
    {
        if (nil_type.get() == nullptr)
            nil_type = std::make_unique<NilType>();

        return nil_type.get();
    }

    BoolType* TypeInterner::GetBoolType()
    {
        if (bool_type.get() == nullptr)
            bool_type = std::make_unique<BoolType>();

        return bool_type.get();
    }

    IntType* TypeInterner::GetIntType()
    {
        if (int_type.get() == nullptr)
            int_type = std::make_unique<IntType>();

        return int_type.get();
    }
    
    FunctionType* TypeInterner::GetFunctionType(Type* r, std::vector<Type*> a)
    {
    	std::unique_ptr<FunctionType> possibility = std::make_unique<FunctionType>(r, a);
    
    	for (auto& ft : function_types)
    	{
    		if (possibility->EqualTo(ft.get()))
    		{
    			return ft.get();
    		}
    	}
    	
    	FunctionType* result = possibility.get();
    	function_types.emplace_back(std::move(possibility));
    	
    	return result;
    }

    PointerType* TypeInterner::GetPointerType(Type* pointee_type)
    {
      std::unique_ptr<PointerType> possible = std::make_unique<PointerType>(pointee_type);

      for (auto& pt : pointer_types)
      {
        if (possible->EqualTo(pt.get()))
          return pt.get();
      }

      PointerType* result = possible.get();
      pointer_types.emplace_back(std::move(possible));

      return result;
    }

    ArrayType* TypeInterner::GetArrayType(size_t size, Type* member_type)
    {
      std::unique_ptr<ArrayType> possible = std::make_unique<ArrayType>(size, member_type);

      for (auto& pt : array_types)
      {
        if (possible->EqualTo(pt.get()))
          return pt.get();
      }

      ArrayType* result = possible.get();
      array_types.emplace_back(std::move(possible));

      return result;
    }

    TupleType* TypeInterner::GetTupleType(std::vector<Type*> member_types)
    {
      std::unique_ptr<TupleType> possible = std::make_unique<TupleType>(member_types);

      for (auto& tt : tuple_types)
      {
        if (possible->EqualTo(tt.get()))
          return tt.get();
      }

      TupleType* result = possible.get();
      tuple_types.emplace_back(std::move(possible));
      return result;
    }
}
