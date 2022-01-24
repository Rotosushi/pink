#pragma once
#include <string>

#include "ast/Ast.h"

namespace pink {
    class Int : public Ast {
    public:
        int value;

        Int(Location l, int i);
        virtual ~Int();

        virtual std::unique_ptr<Ast> Clone() override;

        static bool classof(const Ast* a);

        virtual std::string ToString() override;

        virtual Outcome<Type*, Error> Getype(Environment& e) override;
        // virtual std::variant<pink::Error, llvm::Value*> ToLLVM(Environment& e);
    };
}
