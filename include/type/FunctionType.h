#pragma once 
#include <vector>

#include "type/Type.h"

namespace pink {
	class FunctionType : public Type {
	public:
		Type* result;
		std::vector<Type*> arguments;
	
		FunctionType(Type* r, std::vector<Type*> a);
		virtual ~FunctionType();
		
		static bool classof(const Type* t);
	
		virtual bool EqualTo(Type* other) override;
		virtual std::string ToString() override;
		
		virtual Outcome<llvm::Type*, Error> Codegen(const Environment& env) override;
	};
}
