#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Utilities.h"

char* dupnstr(char* str, int len)
{
  char* result = (char*)calloc(sizeof(char), len + 1);
  result = strncpy(result, str, len);
  return result;
}

char* dupstr(char* str)
{
  return dupnstr(str, strlen(str));
}

/*
  if you call strkat passing in an uninitialized
  char** for the context pointer, it segfaults
  (buidling with clang and gcc).

  if you pass in the address of an uninitialized
  char* however, you don't crash!

  what the fuck?

  it's it semantically identical?

  the only difference should be passing
  the same char** each time, or passing
  a different unnammed char** each time.
  either way it should be the same char*
  across each function call right?

  anyways, it works now so ...
*/
void strkat(char* dest, char* source, char** strctx)
{
  int i = 0, j = 0;
  if (dest != NULL)
  {
    // find the end of the string we are appending to.
    // eww, an unavoidable O(n) operation before we can
    // concatenate the strings....
    while (dest[i] != '\0') i++;

    while (source[j] != '\0')
    {
      dest[i]  = source[j];
      i++;
      j++;
    }
    // if source[i] == '\0'
    // then dest[i] should be
    // made to equal '\0'
    dest[i] = '\0';
    *strctx = &(dest[i]);
  }
  else
  {
    // don't just crash given bad input!
    if (NULL == *strctx) return;

    // assume we are being called after a first call,
    // and therefore the context pointer points to
    // the next place in the destination string
    while (source[j] != '\0')
    {
      // unwrap, unwrap -> char
      *(*strctx) = source[j];
      // unwrap -> char*
      (*strctx)++;
      j++;
    }

    *(*strctx) = '\0';
  }
}

// this is a super straightforward implementation.
char* appendstr(char* s1, char* s2)
{
  // oh nooooo, named intermediary values that would
  // need to be allocated anyways.
  // but the program can optimize intermediary
  // values you say, it can optimize this code too!
  size_t s1sz  = strlen(s1), s2sz = strlen(s2);
  size_t sz    = s1sz + s2sz;
  char* result = (char*)calloc(sizeof(char), sz + 1), *ctx;
  strkat(result, s1, &ctx);
  strkat(NULL,   s2, &ctx);
  return result;
}


char* gensym(char* prefix)
{
  static int counter = 0;
  int prelen = 1;
  char * result = NULL;
  if (prefix)
  {
    prelen = strlen(prefix);
    result = (char*)malloc((sizeof(int) * 8) + prelen + 1);
    sprintf(result, "%s%d", prefix, counter);
  }
  else
  {
    result = (char*)malloc((sizeof(int) * 8) + prelen + 1);
    sprintf(result, "p%d", counter);
  }
  counter++;
  return result;
}
