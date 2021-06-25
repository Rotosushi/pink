#pragma once

#include <string>

#include "llvm/ADT/StringSet.h"

namespace pink {

typedef const char * InternedString;

// so we can intern names and operators with the
// same basic structure. and use both as indexes into
// the mapping of names to values, and operators to
// operator consumption procedures.
class StringInterner
{
private:
  llvm::StringSet<llvm::MallocAllocator> strs;
public:
  // converts the rich interface that StringSet
  // provides into the single action that this
  // class provides for the rest of the program.
  InternedString InternString(const std::string& str);
};

}
