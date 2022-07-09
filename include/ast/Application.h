#pragma once
#include <vector>

#include "ast/Ast.h"

namespace pink {
  class Application : public Ast {
  private:
    std::unique_ptr<Ast> callee;
    std::vector<std::unique_ptr<Ast>> arguments;

    virtual Outcome<Type*, Error> GetypeV(std::shared_ptr<Environment> env) override;
  public:
    Application(Location location, std::unique_ptr<Ast> callee, std::vector<std::unique_ptr<Ast>> arguments);
    ~Application();
  
    static bool classof(const Ast* ast);

    virtual std::unique_ptr<Ast> Clone() override;
    virtual std::string ToString() override;

    virtual Outcome<llvm::Value*, Error> Codegen(std::shared_ptr<Environment> env) override;
  };
}
