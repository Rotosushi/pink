#ifndef NIL_H
#define NIL_H

typedef struct Nil
{
  void* value;
} Nil;

void DestroyNil(Nil* nil);

Nil* CloneNil(Nil* nil);

char* ToStringNil(Nil* nil);

#endif // !NIL_H
