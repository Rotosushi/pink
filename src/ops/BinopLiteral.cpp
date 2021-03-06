
#include "ops/BinopLiteral.h"

namespace pink {
    BinopLiteral::BinopLiteral(Precedence p, Associativity a)
        : precedence(p), associativity(a)
    {

    }

    BinopLiteral::BinopLiteral(Precedence p, Associativity a, Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn)
    : precedence(p), associativity(a)
    {
        overloads.insert(std::make_pair(std::make_pair(left_t, right_t), std::make_unique<BinopCodegen>(ret_t, fn)));
    }

    BinopLiteral::~BinopLiteral()
    {
        // BinopLiteral Manages the memory for the BinopCodegens held in the table
    }

    std::pair<std::pair<Type*, Type*>, BinopCodegen*> BinopLiteral::Register(Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn)
    {
        auto iter = overloads.find(std::make_pair(left_t, right_t));

        if (iter == overloads.end())
        {
            auto pair = overloads.insert(std::make_pair(std::make_pair(left_t, right_t), std::make_unique<BinopCodegen>(ret_t, fn)));
            return std::make_pair(pair.first->first, pair.first->second.get());
        }
        else
        {
            return std::make_pair(iter->first, iter->second.get());
        }
    }

    void BinopLiteral::Unregister(Type*  left_t, Type* right_t)
    {
        auto iter = overloads.find(std::make_pair(left_t, right_t));

        if (iter != overloads.end())
        {
            overloads.erase(iter);
        }
    }

    llvm::Optional<std::pair<std::pair<Type*, Type*>, BinopCodegen*>> BinopLiteral::Lookup(Type* left_t, Type* right_t)
    {
        auto iter = overloads.find(std::make_pair(left_t, right_t));

        if (iter == overloads.end())
            return llvm::Optional<std::pair<std::pair<Type*, Type*>, BinopCodegen*>>();
        else
            return llvm::Optional<std::pair<std::pair<Type*, Type*>, BinopCodegen*>>(std::make_pair(iter->first, iter->second.get()));
    }
}
