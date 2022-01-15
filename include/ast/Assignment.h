
#include "ast/Ast.h"


namespace pink {
    class Assignment : public Ast {
    public:
        Ast* left;
        Ast* right;

        Assignment(Location loc, Ast* left, Ast* right);
        ~Assignment();
        
        static bool classof(const Ast* ast);

        virtual Ast* Clone() override;
        virtual std::string ToString() override;
    };
}
