#ifndef NIL_H
#define NIL_H

struct Object;

typedef struct Nil
{
  void* value;
} Nil;

void InitializeNil(Nil* nil);

void DestroyNil(Nil* nil);

void CloneNil(Nil* destination, Nil* source);

char* ToStringNil(Nil* nil);

#endif // !NIL_H
