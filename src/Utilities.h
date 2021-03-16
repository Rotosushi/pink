#ifndef UTILITIES_H
#define UTILITIES_H

// why isn't strndup a stdlib procedure?
// it can be built entirely out of other
// standard library procedures
char* dupnstr(char* str, int len);

char* dupstr(char* str);

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
void  strkat(char* dest, char* source, char** strctx);


// so, this isn't really addition, as it
// isn't associative. i.e. s0 + s1 != s1 + s0
char* appendstr(char* s1, char* s2);


char* gensym(char* prefix);


#endif // !UTILITIES_H
