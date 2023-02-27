#pragma once
#include <charconv>    // std::to_chars, std::from_chars
#include <string_view> // std::string_view

#include "aux/Outcome.h" // pink::Outcome

namespace pink {
template <typename T>
[[nodiscard]] inline auto ToNumber(std::string_view text)
    -> Outcome<T, std::errc> {
  T value;
  auto [ptr, errc]{std::from_chars(text.begin(), text.end(), value)};
  if (errc != std::errc{}) {
    return errc;
  }
  return value;
}
} // namespace pink
