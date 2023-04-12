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

#include "visitor/VisitorResult.h"

namespace pink {
class AddressOf;
class Application;
class Array;
class Assignment;
class Bind;
class Binop;
class Block;
class Boolean;
class IfThenElse;
class Dot;
class Function;
class Integer;
class Nil;
class Subscript;
class Tuple;
class Unop;
class ValueOf;
class Variable;
class While;

class AstVisitor {
public:
  virtual void Visit(AddressOf *address_of) noexcept    = 0;
  virtual void Visit(Application *application) noexcept = 0;
  virtual void Visit(Array *array) noexcept             = 0;
  virtual void Visit(Assignment *assignment) noexcept   = 0;
  virtual void Visit(Bind *bind) noexcept               = 0;
  virtual void Visit(Binop *binop) noexcept             = 0;
  virtual void Visit(Block *block) noexcept             = 0;
  virtual void Visit(Boolean *boolean) noexcept         = 0;
  virtual void Visit(IfThenElse *conditional) noexcept  = 0;
  virtual void Visit(Dot *dot) noexcept                 = 0;
  virtual void Visit(Function *function) noexcept       = 0;
  virtual void Visit(Integer *integer) noexcept         = 0;
  virtual void Visit(Nil *nil) noexcept                 = 0;
  virtual void Visit(Subscript *subscript) noexcept     = 0;
  virtual void Visit(Tuple *tuple) noexcept             = 0;
  virtual void Visit(Unop *unop) noexcept               = 0;
  virtual void Visit(ValueOf *value_of) noexcept        = 0;
  virtual void Visit(Variable *variable) noexcept       = 0;
  virtual void Visit(While *loop) noexcept              = 0;

  AstVisitor() noexcept                                            = default;
  virtual ~AstVisitor() noexcept                                   = default;
  AstVisitor(const AstVisitor &other) noexcept                     = default;
  AstVisitor(AstVisitor &&other) noexcept                          = default;
  auto operator=(const AstVisitor &other) noexcept -> AstVisitor & = default;
  auto operator=(AstVisitor &&other) noexcept -> AstVisitor      & = default;
};

class ConstAstVisitor {
public:
  virtual void Visit(const AddressOf *address_of) const noexcept    = 0;
  virtual void Visit(const Application *application) const noexcept = 0;
  virtual void Visit(const Array *array) const noexcept             = 0;
  virtual void Visit(const Assignment *assignment) const noexcept   = 0;
  virtual void Visit(const Bind *bind) const noexcept               = 0;
  virtual void Visit(const Binop *binop) const noexcept             = 0;
  virtual void Visit(const Block *block) const noexcept             = 0;
  virtual void Visit(const Boolean *boolean) const noexcept         = 0;
  virtual void Visit(const IfThenElse *conditional) const noexcept  = 0;
  virtual void Visit(const Dot *dot) const noexcept                 = 0;
  virtual void Visit(const Function *function) const noexcept       = 0;
  virtual void Visit(const Integer *integer) const noexcept         = 0;
  virtual void Visit(const Nil *nil) const noexcept                 = 0;
  virtual void Visit(const Subscript *subscript) const noexcept     = 0;
  virtual void Visit(const Tuple *tuple) const noexcept             = 0;
  virtual void Visit(const Unop *unop) const noexcept               = 0;
  virtual void Visit(const ValueOf *value_of) const noexcept        = 0;
  virtual void Visit(const Variable *variable) const noexcept       = 0;
  virtual void Visit(const While *loop) const noexcept              = 0;

  ConstAstVisitor() noexcept                             = default;
  virtual ~ConstAstVisitor() noexcept                    = default;
  ConstAstVisitor(const ConstAstVisitor &other) noexcept = default;
  ConstAstVisitor(ConstAstVisitor &&other) noexcept      = default;
  auto operator=(const ConstAstVisitor &other) noexcept
      -> ConstAstVisitor & = default;
  auto operator=(ConstAstVisitor &&other) noexcept
      -> ConstAstVisitor & = default;
};

} // namespace pink
