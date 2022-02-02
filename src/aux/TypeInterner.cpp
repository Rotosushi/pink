
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
}
