#pragma once
#include <string>

#include "ast/Ast.h"

namespace pink {
    class Bool : public Ast {
    public:
        bool value;

        Bool(Location l, bool b);
        virtual ~Bool();

        virtual Ast* Clone() override;

        static bool classof(const Ast* a);

        virtual std::string ToString() override;

        // virtual std::variant<pink::Error, pink::Type*> Getype(Environment& e);
        // virtual std::variant<pink::Error, llvm::Value*> ToLLVM(Environment& e);
    };
}
