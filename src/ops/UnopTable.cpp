
#include "ops/UnopTable.h"

namespace pink {
    UnopTable::UnopTable()
        : table()
    {

    }

    UnopTable::~UnopTable()
    {
        /*
            UnopTable manages the memory for the Unop Literals
            that are registered through std::unique_ptrs.
        */
    }

    std::pair<InternedString, UnopLiteral*> UnopTable::Register(InternedString op)
    {
        auto iter = table.find(op);

        if (iter != table.end())
        {
            return std::make_pair(iter->first, iter->second.get());
        }
        else
        {
            auto pair = table.insert(std::make_pair(op, std::make_unique<UnopLiteral>()));
            return std::make_pair(pair.first->first, pair.first->second.get());
        }
    }

    std::pair<InternedString, UnopLiteral*> UnopTable::Register(InternedString op, Type* arg_t, Type* ret_t, UnopCodegenFn fn)
    {
        auto iter = table.find(op);

        if (iter != table.end())
        {
            iter->second->Register(arg_t, ret_t, fn);
            return std::make_pair(iter->first, iter->second.get());
        }
        else
        {
            auto pair = table.insert(std::make_pair(op, std::make_unique<UnopLiteral>(arg_t, ret_t, fn)));
            return std::make_pair(pair.first->first, pair.first->second.get());
        }
    }

    void UnopTable::Unregister(InternedString op)
    {
        auto iter = table.find(op);

        if (iter != table.end())
        {
            /*
                UnopTable manages the memory for the Unop Literals
                that are registered through std::unique_ptrs.
            */
            table.erase(iter);
        }
    }

    llvm::Optional<std::pair<InternedString, UnopLiteral*>> UnopTable::Lookup(InternedString op)
    {
        auto iter = table.find(op);

        if (iter == table.end())
            return llvm::Optional<std::pair<InternedString, UnopLiteral*>>();
        else
            return llvm::Optional<std::pair<InternedString, UnopLiteral*>>(std::make_pair(iter->first, iter->second.get()));
    }
}
