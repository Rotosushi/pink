// Copyright (C) 2023 cadence
// 
// This file is part of pink.
// 
// pink is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// pink is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with pink.  If not, see <http://www.gnu.org/licenses/>.

#pragma once
#include <utility>

namespace pink {

/**
 * @brief Essentially std::adjacent_find except it get pairs from
 * two ranges, instead of two adjacent elements in the same range.
 * this operation really only makes sense if the two
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