#pragma once
#include <string>

#include "aux/StringInterner.hpp"
#include "ast/Ast.hpp"

namespace pink {
    class Bind : public Ast {
    public:
        InternedString symbol;
        Ast* term;

        Bind(Location l, InternedString i, Ast* t);
        virtual ~Bind();

        virtual Ast* Clone() override;

        static bool classof(const Ast* a);

        virtual std::string ToString() override;

        // virtual std::variant<pink::Error, pink::Type*> Getype(Environment& e);
        // virtual std::variant<pink::Error, llvm::Value*> ToLLVM(Environment& e);
    };
}
