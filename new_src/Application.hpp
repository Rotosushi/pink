#pragma once
#include <string>
#include <memory>
#include <vector>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"

class Application : public Ast
{
private:
  std::shared_ptr<Ast> function;
  std::vector<std::shared_ptr<Ast>> actual_arguments; 

  virtual Judgement GetypeV(const Environment& env) override;
public:
  Application(const Location& loc, std::shared_ptr<Ast> fn, std::vector<std::shared_ptr<Ast>> args);
  virtual ~Application() = default;

  virtual std::shared_ptr<Application> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Codegen(const Environment& env) override;
};
