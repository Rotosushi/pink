#pragma once
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"

class Application : public Ast
{
private:
  std::shared_ptr<Ast> left;
  std::shared_ptr<Ast> right;

public:
  Application(const Location& loc, std::shared_ptr<Ast> lhs, std::shared_ptr<Ast> rhs);
  virtual ~Application() = default;

  virtual std::shared_ptr<Application> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Getype(const Environment& env) override;
  virtual Judgement Codegen(const Environment& env) override;
};
