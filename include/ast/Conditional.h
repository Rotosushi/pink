#pragma once

#include "ast/Ast.h"



namespace pink {
  class Conditional : public Ast {
  private:
    virtual Outcome<Type*, Error> GetypeV(std::shared_ptr<Environment> env) override;

  public:
    std::unique_ptr<Ast> test;
    std::unique_ptr<Ast> first;
    std::unique_ptr<Ast> second;

    Conditional(Location loc, std::unique_ptr<Ast> test, std::unique_ptr<Ast> first, std::unique_ptr<Ast> second);
    ~Conditional();

    static bool classof(const Ast* ast);

    virtual std::unique_ptr<Ast> Clone() override;
    virtual std::string ToString() override;

    virtual Outcome<llvm::Value*, Error> Codegen(std::shared_ptr<Environment> env) override;
  };
}
