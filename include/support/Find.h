#pragma once
#include <utility>

namespace pink {

/**
 * @brief Essentially std::adjacent_find except it get pairs from
 * two ranges, instead of two adjacent elements in the same range.
 * given that this operation really only makes sense if the two
 * ranges are the same size.
 *
 * @tparam InputIterator the iterator type
 * @tparam BinaryPredicate the comparison function type
 * @param cursor iterator to beginning of first range
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
auto FindBetween(InputIterator cursor, InputIterator end, InputIterator other,
                 BinaryPredicate predicate)
    -> std::pair<InputIterator, InputIterator> {
  while (cursor != end) {
    if (predicate(*cursor, *other)) {
      return {cursor, other};
    }
    cursor++;
    other++;
  }
  return {cursor, other};
}
} // namespace pink