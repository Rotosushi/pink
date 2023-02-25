#pragma once
#include <algorithm> // std::find_if
#include <optional>  // std::optional
#include <utility>   // std::pair
#include <vector>    // std::vector

namespace pink {
template <class K, class V> class Map {
public:
  using Key            = K;
  using Value          = V;
  using Element        = std::pair<Key, Value>;
  using iterator       = typename std::vector<Element>::iterator;
  using const_iterator = typename std::vector<Element>::const_iterator;

private:
  std::vector<Element> map;

public:
  Map() noexcept                                     = default;
  virtual ~Map() noexcept                            = default;
  Map(const Map &other) noexcept                     = delete;
  Map(Map &&other) noexcept                          = default;
  auto operator=(const Map &other) noexcept -> Map & = delete;
  auto operator=(Map &&other) noexcept -> Map      & = default;

  [[nodiscard]] auto size() const noexcept -> std::size_t { return map.size(); }

  auto               begin() noexcept -> iterator { return map.begin(); }
  auto               end() noexcept -> iterator { return map.end(); }
  [[nodiscard]] auto begin() const noexcept -> const_iterator {
    return map.begin();
  }
  [[nodiscard]] auto end() const noexcept -> const_iterator {
    return map.end();
  }

  virtual auto Register(Key key, Value value) -> Element * {
    auto found = Lookup(key);
    if (found.has_value()) {
      return found.value();
    }
    map.emplace_back(std::move(key), std::move(value));
    return std::addressof(map.back());
  }

  // #NOTE: 2/24/2023
  // we have a not const version to allow for a map of maps.
  // otherwise the client could not call Element::Value::Lookup()
  virtual auto Lookup(Key key) -> std::optional<Element *> {
    auto found =
        std::find_if(map.begin(), map.end(), [&](const Element &element) {
          return element.first == key;
        });
    if (found == map.end()) {
      return {};
    }
    return {std::to_address(found)};
  }
};
} // namespace pink