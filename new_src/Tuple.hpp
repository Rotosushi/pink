#pragma once
#include <vector>
#include <array>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Object.hpp"


/*
  class to hold more than one thing in one name.
*/

namespace pink {

class Tuple : public Object
{
private:
  std::vector<std::shared_ptr<Ast>> elements;

  virtual Judgement GetypeV(const Environment& env) override;
public:
  Tuple(const Location& loc, std::initializer_list<std::shared_ptr<Ast>> values);
  Tuple(const Location& loc, std::vector<std::shared_ptr<Ast>> values);
  virtual ~Tuple() = default;

  using iterator = std::vector<std::shared_ptr<Ast>>::iterator;

  iterator begin();
  iterator end();

  virtual std::shared_ptr<Tuple> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Codegen(const Environment& env);
};

}
