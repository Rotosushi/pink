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
#include <optional>

#include "llvm/ADT/DenseMap.h"

namespace llvm {
// provide DenseMapInfo for enums
template <typename Enum>
struct DenseMapInfo<Enum, std::enable_if_t<std::is_enum<Enum>::value>> {
  using UnderlyingType = typename std::underlying_type<Enum>::type;
  using Info           = DenseMapInfo<UnderlyingType>;

  static auto getEmptyKey() -> Enum {
    return static_cast<Enum>(Info::getEmptyKey());
  }

  static auto getTombstoneKey() -> Enum {
    return static_cast<Enum>(Info::getTombstoneKey());
  }

  static auto getHashValue(const Enum &val) -> unsigned {
    return (unsigned)Info::getHashValue(static_cast<UnderlyingType>(val));
  }

  static auto isEqual(const Enum &lhs, const Enum &rhs) -> bool {
    return Info::isEqual(static_cast<UnderlyingType>(lhs),
                         static_cast<UnderlyingType>(rhs));
  }
};
} // namespace llvm

namespace pink {
template <class K, class V> class Map {
public:
  using Key   = K;
  using Value = V;
  using Table = llvm::SmallDenseMap<Key, Value>;

  class Element {
  private:
    typename Table::iterator element;

  public:
    Element(typename Table::iterator element) noexcept
        : element(element) {}
    [[nodiscard]] auto Key() -> Map::Key & { return element->first; }
    [[nodiscard]] auto Key() const -> const Map::Key & {
      return element->first;
    }
    [[nodiscard]] auto Value() -> Map::Value & { return element->second; }
    [[nodiscard]] auto Value() const -> Map::Value & { return element->second; }
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
    return pair.first;
  }

  auto Lookup(Key key) -> std::optional<Element> {
    auto found = table.find(key);
    if (found == table.end()) {
      return {};
    }
    return found;
  }
};
} // namespace pink