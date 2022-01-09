
#include "aux/UnopLiteral.hpp"


namespace pink {
    UnopLiteral::UnopLiteral()
        : overloads()
    {

    }

    UnopLiteral::~UnopLiteral()
    {
        for (auto pair : overloads)
        {
            delete pair.second;
        }
    }

    std::pair<Type*, UnopCodegen*> UnopLiteral::Register(Type* arg_t, Type* ret_t, UnopCodegenFn fn)
    {
        auto pair = overloads.insert(std::make_pair(arg_t, new UnopCodegen(ret_t, fn)));
        return *(pair.first);
    }

    void UnopLiteral::Unregister(Type* arg_t)
    {
        auto iter = overloads.find(arg_t);

        if (iter != overloads.end())
        {
            // UnopLiteral manages the memory of the UnopCodegen instances
            // behind the scenes of the consumer code.
            delete iter->second;
            overloads.erase(iter);
        }
    }

    llvm::Optional<std::pair<Type*, UnopCodegen*>> UnopLiteral::Lookup(Type* arg_t)
    {
        auto iter = overloads.find(arg_t);

        if (iter == overloads.end())
            return llvm::Optional<std::pair<Type*, UnopCodegen*>>();
        else
            return llvm::Optional<std::pair<Type*, UnopCodegen*>>(*iter);

    }
}
