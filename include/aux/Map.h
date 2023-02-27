#pragma once
#include <optional>

#include "llvm/ADT/DenseMap.h"

namespace pink {
template <class K, class V> class Map {
public:
  using Key   = K;
  using Value = V;
  using Table = llvm::SmallDenseMap<Key, Value>;
  // using Element = typename Table::iterator::pointer;
  class Element {
  private:
    typename Table::iterator::pointer element;

  public:
    Element(typename Table::iterator::pointer element) noexcept
        : element(element) {}
    [[nodiscard]] auto Key() -> Map::Key & { return element->getFirst(); }
    [[nodiscard]] auto Key() const -> Map::Key & { return element->getFirst(); }
    [[nodiscard]] auto Value() -> Map::Value & { return element->getSecond(); }
    [[nodiscard]] auto Value() const -> Map::Value & {
      return element->getSecond();
    }
  };

private:
  Table table;

public:
  Map() noexcept
      : table{} {}
  ~Map() noexcept                                    = default;
  Map(const Map &other) noexcept                     = delete;
  Map(Map &&other) noexcept                          = default;
  auto operator=(const Map &other) noexcept -> Map & = delete;
  auto operator=(Map &&other) noexcept -> Map      & = default;

  [[nodiscard]] auto Size() const noexcept -> std::size_t {
    return table.size();
  }

  auto Register(Key key, Value value) -> Element {
    auto pair = table.try_emplace(key, std::move(value));
    return std::to_address(pair.first);
  }

  // #NOTE: 2/24/2023
  // we have a not const version to allow for a map of maps.
  // otherwise the client could not call Element::Value::Lookup()
  auto Lookup(Key key) -> std::optional<Map::Element> {
    auto found = table.find(key);
    if (found == table.end()) {
      return {};
    }
    return std::to_address(found);
  }
};
} // namespace pink