#include "aux/BinopTable.hpp"

namespace pink {
    BinopTable::BinopTable()
        : table()
    {

    }

    BinopTable::~BinopTable()
    {
        for (auto pair : table)
        {
            delete pair.second;
        }
    }

    std::pair<InternedString, BinopLiteral*> BinopTable::Register(InternedString op, Precedence p, Associativity a)
    {
        auto iter = table.find(op);

        if (iter == table.end())
        {
            auto pair = table.insert(std::make_pair(op, new BinopLiteral(p, a)));
            return *(pair.first);
        }
        else
        {
            return *(iter);
        }
    }

    std::pair<InternedString, BinopLiteral*> BinopTable::Register(InternedString op, Precedence p, Associativity a, Type* left_t, Type* right_t, Type* ret_t, BinopCodegenFn fn)
    {
        auto iter = table.find(op);

        if (iter == table.end())
        {
            auto pair = table.insert(std::make_pair(op, new BinopLiteral(p, a, left_t, right_t, ret_t, fn)));
            return *(pair.first);
        }
        else
        {
            // register the new overload to the already registered binop
            // as a convienience for the caller of this procedure.
            iter->second->Register(left_t, right_t, ret_t, fn);
            return *(iter);
        }
    }

    void BinopTable::Unregister(InternedString op)
    {
        auto iter = table.find(op);

        if (iter != table.end())
        {
            delete iter->second;
            table.erase(iter);
        }
    }

    llvm::Optional<std::pair<InternedString, BinopLiteral*>> BinopTable::Lookup(InternedString op)
    {
        auto iter = table.find(op);

        if (iter == table.end())
            return llvm::Optional<std::pair<InternedString, BinopLiteral*>>();
        else
            return llvm::Optional<std::pair<InternedString, BinopLiteral*>>(*iter);
    }
}
