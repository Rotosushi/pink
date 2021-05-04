#pragma once

#include <ostream>
#include <string>

#include "Location.hpp"

class PinkError
{
public:
  std::string dsc;
  Location    loc;

  PinkError(std::string& err, Location& loc);

  void PrintError(std::ostream& out, const std::string& errtxt);
};

void FatalError(const std::string& msg, const std::string& filename, const size_t linenum);
