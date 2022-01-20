#pragma once
#include <string>

#include "ast/Ast.h"
#include "aux/StringInterner.h"

namespace pink {
    class Variable : public Ast {
    public:
        InternedString symbol;

        Variable(Location l, InternedString symbol);
        virtual ~Variable();

        virtual Ast* Clone() override;

        static bool classof(const Ast* a);

        virtual std::string ToString() override;

        virtual Outcome<Type*, Error> Getype(Environment& env) override;
        // virtual std::variant<pink::Error, llvm::Value*> ToLLVM(Environment& e);
    };
}
