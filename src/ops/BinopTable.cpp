#include "ops/BinopTable.h"

namespace pink {
    BinopTable::BinopTable()
        : table()
    {

    }

    BinopTable::~BinopTable()
    { 
    
    }

    std::pair<InternedString, BinopLiteral*> BinopTable::Register(InternedString op, Precedence p, Associativity a)
    {
        auto iter = table.find(op);

        if (iter == table.end())
        {
            auto pair = table.insert(std::make_pair(op, std::make_unique<BinopLiteral>(p, a)));
            return std::make_pair(pair.first->first, pair.first->second.get());
        }
        else
        {
            return std::make_pair(iter->first, iter->second.get());
        }
    }

    std::pair<InternedString, BinopLiteral*> BinopTable::Register(InternedString op, Precedence p, Associativity a, Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn)
    {
        auto iter = table.find(op);

        if (iter == table.end())
        {
            auto pair = table.insert(std::make_pair(op, std::make_unique<BinopLiteral>(p, a, left_t, right_t, ret_t, fn)));
            return std::make_pair(pair.first->first, pair.first->second.get());
        }
        else
        {
            // register the new overload to the already registered binop
            // as a convienience for the caller of this procedure.
            iter->second->Register(left_t, right_t, ret_t, fn);
            return std::make_pair(iter->first, iter->second.get());
        }
    }

    void BinopTable::Unregister(InternedString op)
    {
        auto iter = table.find(op);

        if (iter != table.end())
        {
            table.erase(iter);
        }
    }

    llvm::Optional<std::pair<InternedString, BinopLiteral*>> BinopTable::Lookup(InternedString op)
    {
        auto iter = table.find(op);

        if (iter == table.end())
            return llvm::Optional<std::pair<InternedString, BinopLiteral*>>();
        else
            return llvm::Optional<std::pair<InternedString, BinopLiteral*>>(std::make_pair(iter->first, iter->second.get()));
    }
}
