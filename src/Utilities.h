#ifndef UTILITIES_H
#define UTILITIES_H

// why isn't strndup a stdlib procedure?
// it can be built entirely out of other
// standard library procedures
char* dupnstr(char* str, int len);

/*
  so, you love the c standard string procedure
  strtok right? but hate that it was defined to
  not be reentrant? well, have I got a slightly
  faster implementation of sequential calls to
  strcat for you!
*/
void  strkat(char* dest, char* source, char** strctx);

char* addstr(char* s1, char* s2);


#endif // !UTILITIES_H
