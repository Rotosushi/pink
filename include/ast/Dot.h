#pragma once 
#include "ast/Ast.h"


namespace pink {
  class Dot : public Ast {
  private:
    virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;

  public:
    std::unique_ptr<Ast> left;
    std::unique_ptr<Ast> right;

    Dot(Location location, std::unique_ptr<Ast> left, std::unique_ptr<Ast> right);
    virtual ~Dot();

    static bool classof(const Ast* ast);

    virtual std::string ToString() override;

    virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
  };
}
