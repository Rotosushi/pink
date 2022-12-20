#pragma once
#include <utility> // std::move

namespace pink {
/**
 * @brief Allows the visitor class to "return" a value, which can be
 * polymorphic per visitor class.
 *
 * @tparam Visitor the visitor inheriting from VisitorResult
 * @tparam Ptr the pointer type of this visitor
 * @tparam ResultType the result type of this visitor
 */
template <class Visitor, class Ptr, class ResultType> class VisitorResult {
public:
  /**
   * @brief Calls Accept just like in the normal visitor pattern,
   *  except it can return a result value with templated type.
   *  thus, different visitors can compute different result types.
   *
   * @param ptr the pointer to call Accept on
   * @return ResultType the result of the call to Accept
   */
  inline auto Compute(Ptr ptr, Visitor other) const -> ResultType {
    Visitor visitor(other);
    ptr->Accept(&visitor);
    return visitor.result;
  }

  /**
   * @brief Mimics a polymorphic return type for "Accept"
   *
   * @param result the result value to return.
   */
  inline void Return(ResultType result) const {
    this->result = std::move(result);
  }

private:
  mutable ResultType result;
};
} // namespace pink
