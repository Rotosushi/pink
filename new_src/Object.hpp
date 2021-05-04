#pragma once
#include <string>
#include <memory>

#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"


/*
  Object is fun, it's nothing but
  a kink in the type heirarchy,
  but suddenly you have things to evaluate,
  and things to evaluate upon.
*/
 class Object : public Ast
 {
 public:
   Object(const Location& loc);
   virtual ~Object() = default;

   virtual std::shared_ptr<Object> Clone() = 0;
   virtual std::string ToString() = 0;

   virtual Judgement Getype(const Environment& env) = 0;
   virtual Judgement Codegen(const Environment& env) = 0;
 };
