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

namespace pink {
/*
  Acts as a return slot to a given visitor pattern, which is
  normally unavailable because Accept and Visit both return
  void. and additionally if we give them a return type, that
  type is now mandatory for every visitor. so this class
  gets around that, letting us write multiple visitors which return
  distinct types when processing the same visited object.

  this is an instance of the CRTP (Curiously Reccuring Template Pattern)
*/
template <class Visitor, class Pointer, class ResultType> class VisitorResult {
public:
  ResultType result;

  // #NOTE: 2/18/2023
  // as far as I can tell, passing as a class member via constructors is the
  // only way to pass differing amounts of arguments within any given visitor.
  // because we don't want to modify the signature of Accept or Visit.
  // #NOTE: 2/18/2023
  // can we make compute static?
  auto Compute(Pointer pointer, Visitor *other) -> ResultType {
    Visitor visitor(*other);
    pointer->Accept(&visitor);
    return visitor.result;
  }

  void Return(ResultType value) { result = value; }
};

template <class Visitor, class Pointer, class ResultType>
class ConstVisitorResult {
public:
  /*
    #NOTE: result is mutable because it is acting as the
    return value of the visitor. so even if the visitor
    does something constant, it still needs to modify it's
    return value to communicate it's result.
  */
  mutable ResultType result;

  auto Compute(Pointer pointer, const Visitor *other) const -> ResultType {
    Visitor visitor(*other);
    pointer->Accept(&visitor);
    return visitor.result;
  }

  void Return(ResultType value) const { result = value; }
};
} // namespace pink