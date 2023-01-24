#pragma once

namespace pink {
template <class Visitor, class Pointer, class ResultType> class VisitorResult {
public:
  ResultType result;

  static auto Compute(Pointer pointer) -> ResultType {
    Visitor visitor;
    pointer->Accept(&visitor);
    return visitor.result;
  }

  void Return(ResultType result) { this->result = result; }
};

template <class Visitor, class Pointer, class ResultType>
class ConstVisitorResult {
public:
  mutable ResultType result;

  static auto Compute(Pointer pointer) -> ResultType {
    Visitor visitor;
    pointer->Accept(&visitor);
    return visitor.result;
  }

  void Return(ResultType result) const { this->result = result; }
};
} // namespace pink