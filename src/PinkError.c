#include <stdio.h>
#include <stdlib.h>

#include "Location.h"
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
  // but i don't wuite know how to handle that,
  // and this is already fancyer than the bare minimum,
  // so handling that is left for the future reader of
  // this comment :)
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

  char* restxt = (char*)calloc(sizeof(char), (length + 1));

  strcat(restxt, errtxt);
  strcat(restxt, "\n");
  for (int i = 0; i < txtlen; i++)
  {
    if (i < perr->loc.first_column)
      strcat(restxt, "-");
    else if (i > perr->loc.last_column)
      strcat(restxt, "-");
    else
      strcat(restxt, "^");
  }
  strcat(restxt, "\n");
  strcat(restxt, perr->dsc);
  strcat(restxt, "\n");

  fprintf(out, "%s", restxt);
}

void FatalError(char* msg, const char* file, int line)
{
  fprintf(stderr, "[file:%s, line:%d] %s", file, line, msg);
  exit(1);
}
