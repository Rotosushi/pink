/**
 * @file Array.h
 * @brief Header for class Array
 * @version 0.1
 */
#pragma once
#include <vector>

#include "ast/Ast.h"

#include "llvm/IR/Value.h"

/*
 *  we can imagine a few different syntactic situations where we use
 *  an array
 *
 *  1) declaration and initialization
 *    a) x : [Integer; 5];
 *    b) x := [0, 1, 2, 3, 4];
 *
 *  2) pointer access
 *    a) a := x;
 *    b) b := x[n];
 *    c) c := &x[n];
 *    d) x[n] = 33;
 *    e) *c = 44;
 *
 *  we declare an array of memory in much the same way that
 *  we declare a regular bit of memory, we simply create
 *  an alloca instruction, but we add an argument which is
 *  how many elements to allocate.
 *  in this way, the name bound to an array will act in
 *  the same way as a name bound to any other type.
 *  Array elements can be accessed using the bracket syntax 'a[n]'
 *  and pointers to these elements can be created by using the
 *  address of operator '&a[n]'
 *
 *  There is a slight inconvenience however with the array
 *  type carrying it's size, that is why we are introducing
 *  the slice type. which is a pointer type, which additionally stores an
 *  length + offset, thus a runtime check can be emitted which
 *  bounds checks array access.
 *
 */
namespace pink {

/**
 * @brief Array represents a sequence of known length of a single known type.
 */
class Array : public Ast {
public:
  using Elements       = std::vector<Ast::Pointer>;
  using iterator       = Elements::iterator;
  using const_iterator = Elements::const_iterator;

private:
  Elements elements;

public:
  Array(const Location &location, Elements elements) noexcept
      : Ast(Ast::Kind::Array, location),
        elements(std::move(elements)) {}
  ~Array() noexcept override                             = default;
  Array(const Array &other) noexcept                     = delete;
  Array(Array &&other) noexcept                          = default;
  auto operator=(const Array &other) noexcept -> Array & = delete;
  auto operator=(Array &&other) noexcept -> Array      & = default;

  [[nodiscard]] auto Size() const noexcept -> std::size_t {
    return elements.size();
  }
  [[nodiscard]] auto GetElements() noexcept -> Elements & { return elements; }
  [[nodiscard]] auto GetElements() const noexcept -> const Elements & {
    return elements;
  }

  [[nodiscard]] auto begin() noexcept -> iterator { return elements.begin(); }
  [[nodiscard]] auto begin() const noexcept -> const_iterator {
    return elements.begin();
  }
  [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
    return elements.cbegin();
  }
  [[nodiscard]] auto end() noexcept -> iterator { return elements.end(); }
  [[nodiscard]] auto end() const noexcept -> const_iterator {
    return elements.end();
  }
  [[nodiscard]] auto cend() const noexcept -> const_iterator {
    return elements.cend();
  }

  static auto classof(const Ast *ast) noexcept -> bool {
    return Ast::Kind::Array == ast->GetKind();
  }

  void Accept(AstVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstAstVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
