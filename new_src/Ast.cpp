#include "Location.hpp"
#include "Ast.hpp"

Ast::Ast(const Location& loc) :
  loc(loc)
{}

Location Ast::GetLocation()
{
  return loc;
}
