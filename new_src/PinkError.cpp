#include <ostream>
#include <string>

#include "Location.hpp"
#include "PinkError.hpp"

PinkError::PinkError(std::string& dsc, Location& loc)
{
  this->dsc = dsc;
  this->loc = loc;
}

/*
  https://www.clear.rice.edu/comp506/Labs/IBM-Note.pdf

  inspiration for this subroutine! thanks :)
*/
void PinkError::PrintError(std::ostream& out, const std::string& errtxt)
{
  std::string result;

  result += "\t" + errtxt + "\n";

  for (size_t i = 0; i < errtxt.size(); i++)
  {
    if (i < (4 + this->loc.first_column))
      result += "-";
    else if (i > (4 + this->loc.last_column))
      result += "-";
    else
      result += "^";
  }

  result += "\n\t" + this->dsc + "\n";

  out << result;
}

void FatalError(std::string& msg, const std::string& filename, const size_t linenum)
{
  std::cerr << msg + "[file:" + filename + " line:" std::to_string(linenum) + "]\n";
  exit(1);
}
