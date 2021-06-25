#include <memory>

template <class T>
class Literal : Object
{
private:
  T literal;

public:
  Literal(const Location& loc, const T& literal);
  virtual ~Literal() = default;

  virtual std::shared_ptr<Literal> Clone() override;
  virtual std::string ToString() override;

  virtual Judgement Getype(const Environment& env);
  virtual Judgement Codegen(const Environment& env);
};
