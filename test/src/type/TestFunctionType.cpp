#include <memory>

#include "Test.h"
#include "type/TestFunctionType.h"
#include "type/FunctionType.h"
#include "type/IntType.h"
#include "type/BoolType.h"

bool TestFunctionType(std::ostream& out)
{
	bool result = true;
    out << "\n-----------------------\n";
    out << "Testing Pink::FunctionType: \n";
    
    std::unique_ptr<pink::IntType> it = std::make_unique<pink::IntType>();
	std::unique_ptr<pink::BoolType> bt = std::make_unique<pink::BoolType>();
	
	std::vector<pink::Type*> arg_tys0({it.get(), it.get()});
	
	std::unique_ptr<pink::FunctionType> ft0 = std::make_unique<pink::FunctionType>(bt.get(), arg_tys0);

	result &= Test(out, "FunctionType::getKind", ft0->getKind() == pink::Type::Kind::Function);
	
	result &= Test(out, "FunctionType::classof", ft0->classof(ft0.get()));
	
	std::string ft_str = std::string("Int -> Int -> Bool");
	
	result &= Test(out, "FunctionType::ToString", ft0->ToString() == ft_str);
	


	result &= Test(out, "pink::FunctionType", result);

    out << "\n-----------------------\n";
    return result;
}
