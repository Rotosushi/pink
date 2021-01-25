#include <stdlib.h>
#include <string.h>

#include "StringInterner.h"

StringInterner* CreateStringInterner()
{
  StringInterner* result   = (StringInterner*)malloc(sizeof(StringInterner));
  result->length           = 0;
  result->interned_strings = NULL;
  return result;
}

void DestroyStringInterner(StringInterner* interner)
{
  if (interner == NULL)
    return;

  for (int i = 0; i < interner->length; i++)
  {
    free(interner->interned_strings[i]);
  }
  free(interner->interned_strings);
}

// inserts the name into the set of interned strings
// returns a pointer to the newly inserted string,
// if the name already exists, we simply return a pointer
// to the new result.
InternedString InternString(StringInterner* interner, char* string)
{
  if (interner == NULL || string == NULL)
    return (InternedString)NULL;

  for (int i = 0; i < interner->length; i++)
  {
    char* cur = (interner->interned_strings)[i];
    int cmp = strcmp(cur, string);

    if (cmp == 0)
      return (InternedString)cur;
  }

  // if we got here then the string was not a member
  // of the interned set. so we must insert the passed
  // string into the set, and return that string as the
  // single instance of this string.
  // here realloc saves me from having to memcpy myself.
  interner->interned_strings = (char**)realloc(interner->interned_strings, sizeof(char**) * (interner->length + 1));
  // this is normally always an off by one error
  // because of the nature of zero-indexed arrays,
  // however here it occurs immediately after resizing
  // the array to be one larger, and so this operation
  // is safe.
  interner->interned_strings[interner->length] = string;
  interner->length++;
  // string is now the one and only instance of it's
  // exact layout of character data.
  return string;
}

// searches for the interned string by using the pointer values.
// does not insert into the set if the string is not found,
// this procedure simply returns null.
InternedString FindInternedString(StringInterner* interner, InternedString string)
{
  if (interner == NULL || string == NULL)
    return (InternedString)NULL;

  for (int i = 0; i < interner->length; i++)
  {
    InternedString cur = (InternedString)((interner->interned_strings)[i]);

    if (cur == string)
      return (InternedString)cur;
  }
  return (InternedString)NULL;
}


InternedString FindInternedText(StringInterner* interner, char* string)
{
  if (interner == NULL || string == NULL)
    return (InternedString)NULL;

  for (int i = 0; i < interner->length; i++)
  {
    char* cur = (interner->interned_strings)[i];
    int cmp = strcmp(cur, string);

    if (cmp == 0)
      return (InternedString)cur;
  }

  return (InternedString)NULL;
}
