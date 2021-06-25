#pragma once

#include <ostream>
#include <string>

#include "Location.hpp"

namespace pink {

class Error
{
public:
  std::string dsc;
  Location    loc;

  Error(std::string& err, Location& loc);

  void PrintError(std::ostream& out, const std::string& errtxt);
};

void FatalError(const std::string& msg, const std::string& filename, const size_t linenum);
