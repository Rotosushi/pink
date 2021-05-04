
#include "Location.hpp"
#include "Judgement.hpp"
#include "Environment.hpp"
#include "Ast.hpp"
#include "Object.hpp"

// this function does nothing! >:D
// yet without it, nothing works!
Object::Object(const Location& loc)
  : Ast(loc)
{

}
