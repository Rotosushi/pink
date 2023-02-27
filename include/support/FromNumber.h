#pragma once
#include <array>       // std::array<T, N>
#include <charconv>    // std::to_chars, std::from_chars
#include <concepts>    // std::integral
#include <limits>      // std::numeric_limits<T>::digits10
#include <string_view> // std::string_view

#include "aux/Outcome.h" // pink::Outcome

namespace pink {
template <std::integral T>
[[nodiscard]] inline auto FromNumber(T number)
    -> Outcome<std::string, std::errc> {
  std::array<char, std::numeric_limits<T>::digits10 + 1> buffer{};
  auto [ptr, errc]{std::to_chars(buffer.begin(), buffer.end(), number)};
  if (errc != std::errc{}) {
    return errc;
  }
  return std::string(buffer.begin());
}

template <std::floating_point T>
[[nodiscard]] inline auto FromNumber(T number)
    -> Outcome<std::string, std::errc> {
  std::array<char, std::numeric_limits<T>::max_digits10 + 1> buffer{};
  auto [ptr, errc] = std::to_chars(buffer.begin(), buffer.end(), number);
  if (errc != std::errc{}) {
    return errc;
  }
  return std::string(buffer.begin());
}
} // namespace pink
