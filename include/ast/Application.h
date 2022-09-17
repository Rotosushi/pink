#pragma once
#include <vector>

#include "ast/Ast.h"

namespace pink {
  class Application : public Ast {
  private:
    std::unique_ptr<Ast> callee;
    std::vector<std::unique_ptr<Ast>> arguments;

    virtual Outcome<Type*, Error> GetypeV(const Environment& env) override;
  public:
    Application(Location location, std::unique_ptr<Ast> callee, std::vector<std::unique_ptr<Ast>> arguments);
    ~Application();
  
    static bool classof(const Ast* ast);

    virtual std::string ToString() override;

    virtual Outcome<llvm::Value*, Error> Codegen(const Environment& env) override;
  };
}
