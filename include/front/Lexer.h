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

/**
 * @file Lexer.h
 * @brief header for the Lexer class
 * @version 0.1
 *
 */
#pragma once
#include <istream>
#include <string>
#include <string_view>

#include "aux/Location.h"
#include "front/Token.h"

/*
  #TODO: 3/18/2023 Add comments to the lexer.
  though, is the lexer really the place for them?
  and does the parser then have to handle the possibility 
  or comments within the language?
*/

namespace pink {
/**
 * @brief the Lexer converts an input sequence of characters into an output
 * sequence of [Token](#Token)s
 *
 * The source code of the Lexer itself is generated by [re2c] from an re2c file
 * describing the operation of the Lexer in regular expressions. ()
 *
 * [re2c]: https://re2c.org/manual/manual_c.html
 */
class Lexer {
private:
  Location location;
  std::string buffer;
  std::string::iterator end;
  std::string::iterator cursor;
  std::string::iterator marker;
  std::string::iterator token;

  void UpdateLocation();

public:
  Lexer();
  Lexer(std::string_view text);
  ~Lexer()                                      = default;
  Lexer(const Lexer &other)                     = delete;
  Lexer(Lexer &&other)                          = default;
  auto operator=(const Lexer &other) -> Lexer & = delete;
  auto operator=(Lexer &&other) -> Lexer      & = default;

  [[nodiscard]] auto GetBufferView() const -> std::string_view {
    return buffer;
  }
  void SetBuffer(std::string_view text);
  void AppendToBuffer(std::string_view text);
  void Reset();

  [[nodiscard]] auto EndOfInput() const -> bool;

  /**
   * @brief Get the *next* Token from the buffer.
   *
   * \warning This function *advances* the internal state of the Lexer to
   * point to the next token from the buffer. The Lexer *does not* save the
   * previous Location, text, or Token it was looking at, so
   * be sure you have saved the current Token, text and Location if that
   * is necessary.
   *
   * @return Token the *next* token from the buffer
   */
  auto lex() -> Token;
  auto txt() -> std::string_view;
  auto loc() -> Location;
};
} // namespace pink
