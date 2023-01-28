#pragma once

namespace pink {
/*
  Acts as a return slot to a given visitor pattern, which is
  normally unavailable because Accept and Visit both return
  void. and additionally if we give them a return type, that
  type is now mandatory for every visitor. so this class
  gets around that, letting us write multiple visitors which return
  distinct types when processing the same visited object.

  this is an instance of the CRTP (Curiously Reccuring Template Pattern)
*/
template <class Visitor, class Pointer, class ResultType> class VisitorResult {
public:
  ResultType result;

  // as far as I can tell, passing as a class member via constructors is the
  // only way to pass differing amounts of arguments within any given visitor.
  // because we don't want to modify the signature of Accept or Visit.
  auto Compute(Pointer pointer, Visitor *other) -> ResultType {
    Visitor visitor(*other);
    pointer->Accept(&visitor);
    return visitor.result;
  }

  void Return(ResultType result) { this->result = result; }
};

template <class Visitor, class Pointer, class ResultType>
class ConstVisitorResult {
public:
  /*
    #NOTE: result is mutable because it is acting as the
    return value of the visitor. so even if the visitor
    does something constant, it still needs to modify it's
    return value to communicate it's result.
  */
  mutable ResultType result;

  auto Compute(Pointer pointer, const Visitor *other) const -> ResultType {
    Visitor visitor(*other);
    pointer->Accept(&visitor);
    return visitor.result;
  }

  void Return(ResultType result) const { this->result = result; }
};
} // namespace pink