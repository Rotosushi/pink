#ifndef STRINGINTERNER_H
#define STRINGINTERNER_H

#include <cstdint>
#include <optional>
#include <vector>
using std::vector;
#include <string>
using std::string;

typedef string* InternedString;

class StringInterner
{
private:
  vector<string> interned_strings;
public:
  InternedString InternString(const string* str);
  std::optional<InternedString> FindString(const string* str);
};

#endif // !STRINGINTERNER_H
