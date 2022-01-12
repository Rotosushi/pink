
#include "ops/UnopTable.hpp"

namespace pink {
    UnopTable::UnopTable()
        : table()
    {

    }

    UnopTable::~UnopTable()
    {
        /*
            UnopTable manages the memory for the Unop Literals
            that are registered.
        */
        for (auto pair : table)
        {
            delete pair.second;
        }
    }

    std::pair<InternedString, UnopLiteral*> UnopTable::Register(InternedString op)
    {
        auto iter = table.find(op);

        if (iter != table.end())
        {
            return *iter;
        }
        else
        {
            auto pair = table.insert(std::make_pair(op, new UnopLiteral()));
            return *(pair.first);
        }
    }

    std::pair<InternedString, UnopLiteral*> UnopTable::Register(InternedString op, Type* arg_t, Type* ret_t, UnopCodegenFn fn)
    {
        auto iter = table.find(op);

        if (iter != table.end())
        {
            iter->second->Register(arg_t, ret_t, fn);
            return *iter;
        }
        else
        {
            auto pair = table.insert(std::make_pair(op, new UnopLiteral(arg_t, ret_t, fn)));
            return *(pair.first);
        }
    }

    void UnopTable::Unregister(InternedString op)
    {
        auto iter = table.find(op);

        if (iter != table.end())
        {
            /*
                UnopTable manages the memory for the Unop Literals
                that are registered.
            */
            delete iter->second;
            table.erase(iter);
        }
    }

    llvm::Optional<std::pair<InternedString, UnopLiteral*>> UnopTable::Lookup(InternedString op)
    {
        auto iter = table.find(op);

        if (iter == table.end())
            return llvm::Optional<std::pair<InternedString, UnopLiteral*>>();
        else
            return llvm::Optional<std::pair<InternedString, UnopLiteral*>>(*iter);
    }
}
