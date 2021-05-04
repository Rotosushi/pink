#ifndef UTILITIES_H
#define UTILITIES_H

// why isn't strndup a stdlib procedure?
// it can be built entirely out of other
// standard library procedures
char* dupnstr(const char* str, int len);

char* dupstr(const char* str);

/*
  so, you love the c standard string procedure
  strtok right? but hate that it was defined to
  not be reentrant? well, have I got a slightly
  faster implementation of repeated calls to
  strcat on the same destination buffer for you!

  including the exact same restictions of strcat itself,
  don't pass in pointers to the same array in both
  dest and source and dest needs to already have enough
  allocated space for both source, and the final '\0'

*/
void  strkat(char* dest, const char* source, char** strctx);


// so, this isn't really addition, as it
// isn't associative. i.e. s0 + s1 != s1 + s0
char* appendstr(const char* s1, const char* s2);

// used to be used during evaluation to handle
// name conflics, however we use scopesets to
// distinguish names now, and so we don't currently
// use this procedure anymore. (it is still useful
// i think, so it's staying in for some amount of time.)
char* gensym(const char* prefix);


#endif // !UTILITIES_H
