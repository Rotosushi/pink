#include <stdio.h>
#include <stdlib.h>

#include "Location.h"
#include "Utilities.h"
#include "PinkError.h"

/*
  https://www.clear.rice.edu/comp506/Labs/IBM-Note.pdf

  inspiration for this subroutine! thanks :)
*/
void PrintError(FILE* out, PinkError* perr, char* errtxt)
{
  if (!out)
    FatalError("NULL ouput file", __FILE__, __LINE__);

  if (!perr)
    FatalError("NULL error", __FILE__, __LINE__);

  if (!errtxt)
    FatalError("NULL erroroneous text", __FILE__, __LINE__);
  // this procedure really only makes sense if
  // the errtxt doesn't itself contain a newline
  // but i don't really know how to handle that,
  // and this is already fancyer than the bare minimum,
  // so handling that is left for the future reader of
  // this comment :) (you know, future me, that badass girl! :)
  // output:
  // some-line-of-input-text-which-failed
  // -------------^^^^^------------------
  // a-line-of-text-describing-the-error-found

  // given we need to hold at least as many more characters
  // to hold the little bit of ascii art as there are characters
  // within the erroneous text, the length is multiplied before
  // adding. instead of simply doubling the final value, which
  // while "safe" uses way more space than nesessary.
  int txtlen = strlen(errtxt);
  int length = (txtlen * 2) + strlen(perr->dsc) + 5;

  char* restxt = (char*)calloc(sizeof(char), (length + 1)), *ctx;

  strkat(restxt, errtxt, &ctx);
  strkat(restxt, "\n",   &ctx);
  for (int i = 0; i < txtlen; i++)
  {
    if (i < perr->loc.first_column)
      strkat(restxt, "-", &ctx);
    else if (i > perr->loc.last_column)
      strkat(restxt, "-", &ctx);
    else
      strkat(restxt, "^", &ctx);
  }
  strkat(restxt, "\n",      &ctx);
  strkat(restxt, perr->dsc, &ctx);
  strkat(restxt, "\n",      &ctx);

  fprintf(out, "%s", restxt);
}

void FatalError(char* msg, const char* file, int line)
{
  fprintf(stderr, "[file:%s, line:%d] %s", file, line, msg);
  exit(1);
}
