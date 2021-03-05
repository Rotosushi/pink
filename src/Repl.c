#include <stdio.h>
#include <string.h>

#include "Ast.h"
#include "Judgement.h"
#include "Parser.h"
#include "Environment.h"
#include "Repl.h"

// acts like GNU getline. but without tying us to
// the OS. (even though i could absolutely #include
// the requisite file to get the library version of
// getline.)
size_t getline(char** buffer, size_t* length, FILE* in)
{
  static int resize_factor = 1;
  size_t charsRead = 0;

  if (!length || !(*length))
    return 0;

  *buffer = (char*)realloc(*buffer, sizeof(char) * (*length));

  for (size_t i = 0; i < (*length); i++)
  {
    char current = fgetc(in);
    charsRead++;
    // if the buffer is too small to hold the next
    // character we add more memory to the buffer.
    if ((i + 1) >= (*length))
    {
      // first we add ten more slots,
      // then 100, then 1000. then
      // we just keep adding thousands.
      (*length) += 10 * resize_factor;

      if (resize_factor <= 100)
        resize_factor *= 10;

      *buffer = (char*)realloc(*buffer, sizeof(char) * (*length));
    }

    if (current == '\n' || current == EOF)
    {
      (*buffer)[i]   = current;
      (*buffer)[i+1] = '\0';
      break;
    }
    else
    {
      (*buffer)[i] = current;
    }
  }

  return charsRead;
}

void Repl(FILE* in, FILE* out, Environment* env)
{
  char*  buffer = (char*)malloc(sizeof(char) * 100);
  size_t length = 100;

  while (true)
  {
    fprintf (out, "> ");
    size_t charsRead = getline(&buffer, &length, in);

    Judgement parsejdgmt = Parse(env->parser, buffer);

    if (parsejdgmt.success == true)
    {
      char* termtxt = ToStringAst(parsejdgmt.term);
      fprintf(out, "parsed:[%s]\n", termtxt);
      free(termtxt);

      Judgement typejdgmt = Getype(parsejdgmt.term, env);

      if (typejdgmt.success == true)
      {
        fprintf(out, "type:[%s]\n", ToStringType(typejdgmt.type));
      }
      else
      {
        PrintError(out, &(typejdgmt.error), buffer);
      }

    }
    else
    {
      PrintError(out, &(parsejdgmt.error), buffer);
    }
    if (buffer)
    {
      free(buffer);
      buffer = NULL;
    }
    buffer = (char*)malloc(sizeof(char) * 100);
    length = 100;
  }
}
