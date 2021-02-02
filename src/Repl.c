#include <stdio.h>
#include <string.h>

#include "Ast.h"
#include "ParseJudgement.h"
#include "Parser.h"
#include "Environment.h"
#include "Repl.h"

size_t getline(char** buffer, size_t* length, FILE* in)
{
  size_t charsRead = 0;
  *buffer = (char*)realloc(*buffer, sizeof(char) * (*length));

  for (size_t i = 0; i < (*length); i++)
  {
    char current = fgetc(in);

    if ((i + 1) >= (*length))
    {
      (*length) += 10;
      *buffer    = (char*)realloc(*buffer, sizeof(char) * (*length));
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

    ParseJudgement parsejdgmt = Parse(env->parser, buffer);

    if (parsejdgmt.success == true)
    {
      fprintf(out, "%s\n", ToStringAst(parsejdgmt.term));
    }
    else
    {
      PrintError(out, &(parsejdgmt.error), buffer);
    }
  }
}
