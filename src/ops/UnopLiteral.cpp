
#include "ops/UnopLiteral.h"


namespace pink {
    UnopLiteral::UnopLiteral()
        : overloads()
    {

    }

    UnopLiteral::UnopLiteral(Type* arg_t, Type* ret_t, UnopCodegenFn fn)
        : overloads()
    {
        overloads.insert(std::make_pair(arg_t, std::make_unique<UnopCodegen>(ret_t, fn)));
    }

    UnopLiteral::~UnopLiteral()
    {
    
    }

    std::pair<Type*, UnopCodegen*> UnopLiteral::Register(Type* arg_t, Type* ret_t, UnopCodegenFn fn)
    {
        auto pair = overloads.insert(std::make_pair(arg_t, std::make_unique<UnopCodegen>(ret_t, fn)));
        return std::make_pair(pair.first->first, pair.first->second.get());
    }

    void UnopLiteral::Unregister(Type* arg_t)
    {
        auto iter = overloads.find(arg_t);

        if (iter != overloads.end())
        {
            // UnopLiteral manages the memory of the UnopCodegen instances
            // behind the scenes of the consumer code via std::shared_ptrs
            overloads.erase(iter);
        }
    }

    llvm::Optional<std::pair<Type*, UnopCodegen*>> UnopLiteral::Lookup(Type* arg_t)
    {
        auto iter = overloads.find(arg_t);

        if (iter == overloads.end())
            return llvm::Optional<std::pair<Type*, UnopCodegen*>>();
        else
            return llvm::Optional<std::pair<Type*, UnopCodegen*>>(std::make_pair(iter->first, iter->second.get()));

    }
}
