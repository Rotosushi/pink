
#include "ast/Ast.h"


namespace pink {
    class Assignment : public Ast {
    private:
    	virtual Outcome<Type*, Error> GetypeV(Environment& env) override;
    
    public:
        std::unique_ptr<Ast> left;
        std::unique_ptr<Ast> right;

        Assignment(Location loc, std::unique_ptr<Ast> left, std::unique_ptr<Ast> right);
        ~Assignment();
        
        static bool classof(const Ast* ast);

        virtual std::unique_ptr<Ast> Clone() override;
        virtual std::string ToString() override;
        
       
        virtual Outcome<llvm::Value*, Error> Codegen(Environment& env) override;
    };
}
