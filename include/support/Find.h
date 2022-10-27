#pragma once
#include <utility>

namespace pink {
// This is a naieve implementation of 'std::find_if'
// except that it compares between values held within
// two distinct ranges.
template <class InputIterator, class BinaryPredicate>
auto FindPair(InputIterator begin, InputIterator end, InputIterator other,
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