#ifndef STRINGINTERNER_H
#define STRINGINTERNER_H

typedef char* InternedString;

// this is a space for SSA instructions,
// specifically compare all of these
// pointers (integers) simultaneously and then
// return the match. if no match, insert.
// the same algorithm, just fast.
typedef struct StringInterner
{
  int length;
  char** interned_strings;
} StringInterner;

StringInterner* CreateStringInterner();
void DestroyStringInterner(StringInterner* interner);

// inserts the name into the set of interned strings
// returns a pointer to the newly inserted string,
// if the name already exists, we simply return a pointer
// to the new result.
InternedString InternString(StringInterner* interner, char* string);

// searches for the interned string by using the pointer values.
// does not insert into the set if the string is not found,
// this procedure simply returns the nullptr immediately.
InternedString FindInternedString(StringInterner* interner, InternedString string);

// searched for the string using strcmp.
InternedString FindInternedText(StringInterner* interner, char* string);

#endif // !STRINGINTERNER_H
