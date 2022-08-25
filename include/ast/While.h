#pragma once

#include "ast/Ast.h"

namespace pink {
  class While : public Ast {
  private:
    virtual Outcome<Type*, Error> GetypeV(std::shared_ptr<Environment> env) override;

  public:
    std::unique_ptr<Ast> test;
    std::unique_ptr<Ast> body;

    While(Location l, std::unique_ptr<Ast> test, std::unique_ptr<Ast> body);
    virtual ~While();

    virtual std::unique_ptr<Ast> Clone() override;
    virtual std::string ToString() override;

    static bool classof(const Ast* ast);

    virtual Outcome<llvm::Value*, Error> Codegen(std::shared_ptr<Environment> env) override;
  };
}
