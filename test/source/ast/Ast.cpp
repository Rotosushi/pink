#include "catch2/catch_test_macros.hpp"

#include "ast/All.h"

#include <random>

/*
  #TODO: we are unit testing the Ast nodes
  by storing nullptr's in each of their child
  nodes. This would not catch a bug if the Ast
  node was somehow failing to construct a child
  node with the data we gave it, and instead was
  default constructing it's child nodes.
  So modify the test code to add other Ast nodes
  as the child nodes, and check that the children
  are acctually what we give each node.
*/

static auto RandomLocation() -> pink::Location {
  std::mt19937                          generator;
  std::uniform_int_distribution<size_t> distribution(0);
  return {distribution(generator), distribution(generator),
          distribution(generator), distribution(generator)};
}

static auto IntegerChild() -> pink::Ast::Pointer {
  pink::Location                           location;
  std::mt19937                             generator;
  std::uniform_int_distribution<long long> distribution(0);
  long long                                value = distribution(generator);
  return std::make_unique<pink::Integer>(location, value);
}

TEST_CASE("ast/Application", "[unit][ast]") {
  pink::Location               location = RandomLocation();
  pink::Ast::Pointer           callee   = nullptr;
  pink::Application::Arguments arguments;
  arguments.emplace_back(nullptr);
  arguments.emplace_back(nullptr);
  pink::Ast::Pointer ast = std::make_unique<pink::Application>(
      location, std::move(callee), std::move(arguments));
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Application);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Application>(ast));
  auto *application = llvm::dyn_cast<pink::Application>(ast.get());
  REQUIRE(application != nullptr);
  REQUIRE(application->GetCallee() == nullptr);
  for (const auto &argument : application->GetArguments()) {
    REQUIRE(argument == nullptr);
  }
}

TEST_CASE("ast/Array", "[unit][ast]") {
  pink::Location        location = RandomLocation();
  pink::Array::Elements elements;
  elements.emplace_back(nullptr);
  elements.emplace_back(nullptr);
  elements.emplace_back(nullptr);
  pink::Ast::Pointer ast =
      std::make_unique<pink::Array>(location, std::move(elements));
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Array);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Array>(ast));
  auto *array = llvm::dyn_cast<pink::Array>(ast.get());
  REQUIRE(array != nullptr);
  for (const auto &element : array->GetElements()) {
    REQUIRE(element == nullptr);
  }
}

TEST_CASE("ast/Assignment", "[unit][ast]") {
  pink::Location     location = RandomLocation();
  pink::Ast::Pointer left;
  pink::Ast::Pointer right;
  pink::Ast::Pointer ast = std::make_unique<pink::Assignment>(
      location, std::move(left), std::move(right));
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Assignment);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Assignment>(ast));
  auto *assignment = llvm::dyn_cast<pink::Assignment>(ast.get());
  REQUIRE(assignment != nullptr);
  REQUIRE(assignment->GetLeft() == nullptr);
  REQUIRE(assignment->GetRight() == nullptr);
}

TEST_CASE("ast/Bind", "[unit][ast]") {
  pink::Location       location = RandomLocation();
  pink::InternedString symbol   = "x";
  pink::Ast::Pointer   right;
  pink::Ast::Pointer   ast =
      std::make_unique<pink::Bind>(location, symbol, std::move(right));
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Bind);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Bind>(ast));
  auto *bind = llvm::dyn_cast<pink::Bind>(ast.get());
  REQUIRE(bind != nullptr);
  REQUIRE(bind->GetSymbol() == symbol);
  REQUIRE(bind->GetAffix() == nullptr);
}

TEST_CASE("ast/Binop", "[unit][ast]") {
  pink::Location       location = RandomLocation();
  pink::InternedString op       = "+";
  pink::Ast::Pointer   left;
  pink::Ast::Pointer   right;
  pink::Ast::Pointer   ast = std::make_unique<pink::Binop>(
      location, op, std::move(left), std::move(right));
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Binop);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Binop>(ast));
  auto *binop = llvm::dyn_cast<pink::Binop>(ast.get());
  REQUIRE(binop != nullptr);
  REQUIRE(binop->GetOp() == op);
  REQUIRE(binop->GetLeft() == nullptr);
  REQUIRE(binop->GetRight() == nullptr);
}

TEST_CASE("ast/Block", "[unit][ast]") {
  pink::Location           location = RandomLocation();
  pink::Block::Expressions expressions;
  expressions.emplace_back(nullptr);
  expressions.emplace_back(nullptr);
  expressions.emplace_back(nullptr);
  pink::Ast::Pointer ast =
      std::make_unique<pink::Block>(location, std::move(expressions), nullptr);
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Block);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Block>(ast));
  auto *block = llvm::dyn_cast<pink::Block>(ast.get());
  REQUIRE(block != nullptr);
  for (const auto &expression : block->GetExpressions()) {
    REQUIRE(expression == nullptr);
  }
  // REQUIRE(block->GetScope()->OuterScope() == nullptr);
}

TEST_CASE("ast/Boolean", "[unit][ast]") {
  bool               value    = true;
  pink::Location     location = RandomLocation();
  pink::Ast::Pointer ast = std::make_unique<pink::Boolean>(location, value);
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Boolean);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Boolean>(ast));
  auto *boolean = llvm::dyn_cast<pink::Boolean>(ast.get());
  REQUIRE(boolean != nullptr);
  REQUIRE(boolean->GetValue() == value);
}

TEST_CASE("ast/Conditional", "[unit][ast]") {
  pink::Location     location = RandomLocation();
  pink::Ast::Pointer test;
  pink::Ast::Pointer first;
  pink::Ast::Pointer second;
  pink::Ast::Pointer ast = std::make_unique<pink::Conditional>(
      location, std::move(test), std::move(first), std::move(second));
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Conditional);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Conditional>(ast));
  auto *conditional = llvm::dyn_cast<pink::Conditional>(ast.get());
  REQUIRE(conditional != nullptr);
  REQUIRE(conditional->GetTest() == nullptr);
  REQUIRE(conditional->GetFirst() == nullptr);
  REQUIRE(conditional->GetSecond() == nullptr);
}

TEST_CASE("ast/Dot", "[unit][ast]") {
  pink::Location     location = RandomLocation();
  pink::Ast::Pointer left;
  pink::Ast::Pointer right;
  pink::Ast::Pointer ast =
      std::make_unique<pink::Dot>(location, std::move(left), std::move(right));
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Dot);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Dot>(ast));
  auto *dot = llvm::dyn_cast<pink::Dot>(ast.get());
  REQUIRE(dot != nullptr);
  REQUIRE(dot->GetLeft() == nullptr);
  REQUIRE(dot->GetRight() == nullptr);
}

TEST_CASE("ast/Function", "[unit][ast]") {
  pink::Location            location = RandomLocation();
  pink::InternedString      name     = "f";
  pink::Function::Arguments arguments;
  arguments.emplace_back(nullptr, nullptr);
  arguments.emplace_back(nullptr, nullptr);
  pink::Ast::Pointer body;
  pink::Ast::Pointer ast = std::make_unique<pink::Function>(
      location, name, std::move(arguments), std::move(body), nullptr);
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Function);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Function>(ast));
  auto *function = llvm::dyn_cast<pink::Function>(ast.get());
  REQUIRE(function != nullptr);
  REQUIRE(function->GetName() == name);
  for (const auto &argument : function->GetArguments()) {
    REQUIRE(argument.first == nullptr);
    REQUIRE(argument.second == nullptr);
  }
  REQUIRE(function->GetBody() == nullptr);
  // REQUIRE(function->GetScope()->OuterScope() == nullptr);
}

TEST_CASE("ast/Integer", "[unit][ast]") {
  long long          value    = 42;
  pink::Location     location = RandomLocation();
  pink::Ast::Pointer ast = std::make_unique<pink::Integer>(location, value);
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Integer);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Integer>(ast));
  auto *integer = llvm::dyn_cast<pink::Integer>(ast.get());
  REQUIRE(integer != nullptr);
  REQUIRE(integer->GetValue() == value);
}

TEST_CASE("ast/Nil", "[unit][ast]") {
  pink::Location     location = RandomLocation();
  pink::Ast::Pointer ast      = std::make_unique<pink::Nil>(location);
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Nil);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Nil>(ast));
  REQUIRE(llvm::dyn_cast<pink::Nil>(ast.get()) != nullptr);
}

TEST_CASE("ast/Subscript", "[unit][ast]") {
  pink::Location     location = RandomLocation();
  pink::Ast::Pointer left;
  pink::Ast::Pointer right;
  pink::Ast::Pointer ast = std::make_unique<pink::Subscript>(
      location, std::move(left), std::move(right));
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Subscript);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Subscript>(ast));
  auto *subscript = llvm::dyn_cast<pink::Subscript>(ast.get());
  REQUIRE(subscript != nullptr);
  REQUIRE(subscript->GetLeft() == nullptr);
  REQUIRE(subscript->GetRight() == nullptr);
}

TEST_CASE("ast/Tuple", "[unit][ast]") {
  pink::Location        location = RandomLocation();
  pink::Tuple::Elements elements;
  elements.emplace_back(nullptr);
  elements.emplace_back(nullptr);
  pink::Ast::Pointer ast =
      std::make_unique<pink::Tuple>(location, std::move(elements));
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Tuple);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Tuple>(ast));
  auto *tuple = llvm::dyn_cast<pink::Tuple>(ast.get());
  REQUIRE(tuple != nullptr);
  for (const auto &element : tuple->GetElements()) {
    REQUIRE(element == nullptr);
  }
}

TEST_CASE("ast/Unop", "[unit][ast]") {
  pink::Location       location = RandomLocation();
  pink::InternedString op       = "-";
  pink::Ast::Pointer   right;
  pink::Ast::Pointer   ast =
      std::make_unique<pink::Unop>(location, op, std::move(right));
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Unop);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Unop>(ast));
  auto *unop = llvm::dyn_cast<pink::Unop>(ast.get());
  REQUIRE(unop != nullptr);
  REQUIRE(unop->GetOp() == op);
  REQUIRE(unop->GetRight() == nullptr);
}

TEST_CASE("ast/Variable", "[unit][ast]") {
  pink::Location       location = RandomLocation();
  pink::InternedString symbol   = "x";
  pink::Ast::Pointer   ast = std::make_unique<pink::Variable>(location, symbol);
  REQUIRE(ast->GetKind() == pink::Ast::Kind::Variable);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::Variable>(ast));
  auto *variable = llvm::dyn_cast<pink::Variable>(ast.get());
  REQUIRE(variable != nullptr);
  REQUIRE(variable->GetSymbol() == symbol);
}

TEST_CASE("ast/While", "[unit][ast]") {
  pink::Location     location = RandomLocation();
  pink::Ast::Pointer test;
  pink::Ast::Pointer body;
  pink::Ast::Pointer ast =
      std::make_unique<pink::While>(location, std::move(test), std::move(body));
  REQUIRE(ast->GetKind() == pink::Ast::Kind::While);
  REQUIRE(ast->GetLocation() == location);
  REQUIRE(llvm::isa<pink::While>(ast));
  auto *loop = llvm::dyn_cast<pink::While>(ast.get());
  REQUIRE(loop != nullptr);
  REQUIRE(loop->GetTest() == nullptr);
  REQUIRE(loop->GetBody() == nullptr);
}