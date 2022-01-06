#pragma once
#include <string>

#include "ast/Ast.hpp"
#include "aux/StringInterner.hpp"

namespace pink {
    class Variable : public Ast {
    public:
        InternedString symbol;

        Variable(Location l, InternedString symbol);
        virtual ~Variable();

        virtual Ast* Clone() override;

        static bool classof(const Ast* a);

        virtual std::string ToString() override;

        // virtual std::variant<pink::Error, pink::Type*> Getype(Environment& e);
        // virtual std::variant<pink::Error, llvm::Value*> ToLLVM(Environment& e);
    };
}
