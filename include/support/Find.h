#pragma once
#include <utility>

namespace pink {

/**
 * @brief Essentially std::find_if except that it compares two values
 * where each value is held in a separate range.
 *
 * @tparam InputIterator the iterator type
 * @tparam BinaryPredicate the comparison function type
 * @param begin iterator to beginning of first range
 * @param end iterator to end of second range
 * @param other iterator to beginning of second range
 * @param predicate the comparison function
 * @return std::pair<InputIterator, InputIterator> if the predicate returns true
 * given two values, then this holds the two iterators which returned true.
 * if the predicate never returns true, then this holds iterators to the end of
 * the first range and the iterator to that same position in the second range.
 * (that is if the ranges are the same length, then this holds iterators to the
 * end of both ranges)
 */
template <class InputIterator, class BinaryPredicate>
auto FindBetween(InputIterator begin, InputIterator end, InputIterator other,
                 BinaryPredicate predicate)
    -> std::pair<InputIterator, InputIterator> {
  while (begin != end) {
    if (predicate(*begin, *other)) {
      return {begin, other};
    }
    begin++;
    other++;
  }
  return {begin, other};
}
} // namespace pink